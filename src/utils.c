
#include "utils.h"

size_t ft_strlen(const char *s)
{
	size_t len;

	len = 0;
	while (*s++)
		len++;
	return (len);
}

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char	c1;
	unsigned char	c2;
	int				result;

	result = 0;
	while (n--)
	{
		c1 = (unsigned char)*s1;
		c2 = (unsigned char)*s2;
		result = c1 - c2;
		if (result != 0 || (c1 == '\0' && c2 == '\0'))
			return (result);
		s1++;
		s2++;
	}
	return (0);
}

void *ft_memset(void *b, int c, size_t len)
{
	unsigned char	*memory;
	unsigned char	ch;
	size_t			index;

	memory = (unsigned char*)b;
	ch = (unsigned char)c;
	index = 0;
	while (index < len)
	{
		memory[index] = ch;
		index += 1;
	}
	return (b);
}

void *ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*src_mem;
	unsigned char	*dst_mem;
	size_t			index;

	src_mem = (unsigned char*)src;
	dst_mem = (unsigned char*)dst;
	index = 0;
	while (n--)
	{
		dst_mem[index] = src_mem[index];
		index++;
	}
	return (dst_mem);
}