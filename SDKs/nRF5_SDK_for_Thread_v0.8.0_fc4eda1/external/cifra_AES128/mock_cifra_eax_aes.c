/*lint -e???? -save */
/* AUTOGENERATED FILE. DO NOT EDIT. */
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "unity.h"
#include "cmock.h"
#include "mock_cifra_eax_aes.h"

static struct mock_cifra_eax_aesInstance
{
  unsigned char placeHolder;
} Mock;

extern jmp_buf AbortFrame;
extern int GlobalExpectCount;
extern int GlobalVerifyOrder;

void mock_cifra_eax_aes_Verify(void)
{
}

void mock_cifra_eax_aes_Init(void)
{
  mock_cifra_eax_aes_Destroy();
}

void mock_cifra_eax_aes_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
}

/* lint -restore */
