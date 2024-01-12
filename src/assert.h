#if !defined(ASSERT_H)
#define ASSERT_H

void assert_handle(const char *file, int line);

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            assert_handle(__FILE__, __LINE__); \
        } \
    } while (0)

#endif // ASSERT_H
