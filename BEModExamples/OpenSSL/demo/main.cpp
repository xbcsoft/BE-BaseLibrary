#include "stdafx.h"
#include <openssl/bn.h>

int main()
{
	BIGNUM* a = BN_new();
	BIGNUM* b = BN_new();
	BIGNUM* r = BN_new();

	if (!a || !b || !r) {
		printf("BN_new failed\n");
		return -1;
	}

	// 十进制字符串转 BIGNUM
	BN_dec2bn(&a, "123456789012345678901234567890");
	BN_dec2bn(&b, "987654321098765432109876543210");

	// r = a + b
	if (!BN_add(r, a, b)) {
		printf("BN_add failed\n");
		return -1;
	}

	// 输出结果
	char *result = BN_bn2dec(r);
	if (result) {
		printf("result = %s\n", result);
	}

	BN_free(a);
	BN_free(b);
	BN_free(r);

	return 0;
}