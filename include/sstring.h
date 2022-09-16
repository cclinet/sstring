#ifndef SSTRING_SSTRING_H
#define SSTRING_SSTRING_H

#include <array>
#include <bit>
#include <iostream>
#include <memory>
#include <ostream>


namespace ss {
    constexpr bool IS_LITTLE_ENDIAN = std::endian::native == std::endian::little;
    enum class Category : uint8_t { SMALL = 0b00, MEDIUM = 0b01, LARGE = 0b10 };
    template<class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
    class String {
    public:
        struct Medium {
            CharT *data_;
            size_t size_;
            size_t capacity_;
        };
        struct Large {
            CharT *data_;
            size_t size_;
            size_t capacity_;
        };
        union {
            CharT small_[sizeof(Medium) / sizeof(CharT)];
            Medium medium_;
            Large large_;
            uint8_t bytes_[sizeof(Medium)];
        };
        String() = default;
        explicit String(const CharT *data) {
            auto size = Traits::length(data);
            if (size <= 23) {
                init_small(data, size);
            } else if (size <= 25) {
                init_medium(data, size);
            } else {
                init_large(data, size);
            }
        }
        ~String() {
            Category cat = category();
            switch (cat) {
                case Category::SMALL:
                    break;
                case Category::MEDIUM:
                    destroy_medium();
                    break;
                case Category::LARGE:
                    destroy_large();
                    break;
            }
        }
        friend std::ostream &operator<<(std::ostream &os, const String &s) {
            Category cat = s.category();
            switch (cat) {
                case Category::SMALL: {
                    uint8_t size = s.bytes_[last_byte_pos] >> 2;
                    os.write(s.small_, size);
                    break;
                }
                case Category::MEDIUM: {
                    os << s.medium_.data_;
                    break;
                }
                case Category::LARGE: {
                    os << s.large_.data_;
                    break;
                }
            }

            return os;
        }

        [[nodiscard]] Category category() const {
            uint8_t cat_num = bytes_[last_byte_pos] & 3;
            Category cat{cat_num};
            return cat;
        }

        [[nodiscard]] size_t size() const {
            size_t size;
            Category cat = category();
            switch (cat) {
                case Category::SMALL: {
                    size = bytes_[last_byte_pos] >> 2;
                    break;
                }
                case Category::MEDIUM: {
                    size = medium_.size_;
                    break;
                }
                case Category::LARGE: {
                    size = large_.size_;
                    break;
                }
            }
            return size;
        }

        [[nodiscard]] size_t capacity() const {
            size_t capacity;
            Category cat = category();
            switch (cat) {
                case Category::SMALL: {
                    capacity = sizeof(Medium) / sizeof(CharT);
                    break;
                }
                case Category::MEDIUM: {
                    capacity = medium_.capacity_ >> 2;
                    break;
                }
                case Category::LARGE: {
                    capacity = large_.capacity_ >> 2;
                    break;
                }
            }
            return capacity;
        }

    private:
        static constexpr size_t last_byte_pos = IS_LITTLE_ENDIAN ? sizeof(bytes_) - sizeof(size_t) : sizeof(bytes_) - 1;
        Allocator alloc;

        void init_small(const CharT *data, size_t size) {
            strcpy(small_, data);
            set_small_size(size);
        }
        void set_small_size(size_t size) {
            bytes_[last_byte_pos] = (static_cast<uint8_t>(size) << 2) | static_cast<uint8_t>(Category::SMALL);
        }
        void init_medium(const CharT *data, size_t size) {
            medium_.data_ = alloc.allocate(size + 1);
            strcpy(medium_.data_, data);
            medium_.size_ = size;
            set_capacity(Category::MEDIUM, size);
        }
        void init_large(const CharT *data, size_t size) {
            large_.data_ = alloc.allocate(size + 1);
            strcpy(large_.data_, data);
            medium_.size_ = size;
            set_capacity(Category::LARGE, size);
        }

        void destroy_medium() {
            std::destroy_n(medium_.data_, medium_.capacity_);
            alloc.deallocate(medium_.data_, medium_.capacity_);
        }
        void destroy_large() {
            std::destroy_n(large_.data_, large_.capacity_);
            alloc.deallocate(large_.data_, large_.capacity_);
        }

        void set_capacity(Category cat, uint8_t cap) {
            switch (cat) {
                case Category::SMALL:
                    break;
                case Category::MEDIUM: {
                    medium_.capacity_ = (cap << 2) | static_cast<size_t>(Category::MEDIUM);
                    break;
                }
                case Category::LARGE: {
                    large_.capacity_ = cap;
                    large_.capacity_ = (cap << 2) | static_cast<size_t>(Category::LARGE);
                    break;
                }
            }
        }
    };

    using string = String<char>;
}// namespace ss

#endif//SSTRING_SSTRING_H