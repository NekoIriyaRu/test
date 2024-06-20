#include "LZSS.h"

#include <memory>


namespace AdvHDStaticLibrary
{
	#define MakeStruct_LZSS(offset,count) (((unsigned short)offset << 4) | ((unsigned short)count & 0x000F))
	#define MakeToken_LZSS(token,flag) (((unsigned char)token ^ (0x1 << (7 - flag))) & (unsigned char)token)

	void LZSS_AdvHD_Decode(char* pRaw, char* pDec)
	{
		char aSlide[0x1000] = { 0 };
		for (short iteSlide = 1; true;)
		{
			for (char iteToken = 0, nToken = pRaw++[0]; iteToken < 8; iteToken++, nToken >>= 1)
			{
				if (nToken & 0b00000001)
				{
					pDec[0] = pRaw++[0];
					aSlide[iteSlide] = pDec++[0];

					iteSlide = ++iteSlide & 0x0FFF;
				}
				else
				{
					short off = ((pRaw[0] << 4) & 0x0FFF) + ((pRaw++[1] >> 4) & 0x000F);
					short len = (pRaw++[0] & 0x0F) + 2;
					if (!off) return;

					while (len-- > 0)
					{
						pDec[0] = *(aSlide + off);
						aSlide[iteSlide] = pDec++[0];

						iteSlide = ++iteSlide & 0x0FFF;
						off = ++off & 0x0FFF;
					}
				}
			}
		}
	}

	//I'm tired of writing this LZSS compression algorithm
	//So I choose to give up
	size_t Fake_AdvHD_Encode(char* pRaw, size_t szRaw, char* pEnc)
	{
		char* pEnc_Back = pEnc;
		size_t align = szRaw % 8;
		for (size_t iteRaw = 0; iteRaw < ((szRaw - align) / 8); iteRaw++)
		{
			*(unsigned char*)pEnc = 0xFF;
			memcpy(pEnc + 1, pRaw, 8);

			pEnc += 9;
			pRaw += 8;
		}

		if (align)
		{
			*(unsigned char*)pEnc = 0xFF;
			memcpy(pEnc + 1, pRaw, align);
			pEnc += align + 1;
		}

		pEnc[0] = 0x00;
		pEnc[1] = 0x00;
		pEnc[2] = 0x00;

		return (size_t)(pEnc - pEnc_Back + 3);
	}

/*

LZSS_AdvHD
ǰ��֪ʶ��
{
   Slide:
   {
      Slide��СΪ0x1000����һ��ָ��iteSlide��ʼֵΪ1��
      �����ѹBufferд������ʱ��ͬʱ��Slideд����ͬ�����ݡ�
      �����н�ѹ������0x1000��ʱSlide���ѹ������������ȫͬ����
      ������0x1000������Slide��ָ��Ϊ�㣬��ָ��ͷ��
   }

   Token:
   {
      TokenΪһ���ֽڣ��ܹ���λ�����ڱ�ʾ��������Ƿ���Ҫչ��������ԭʼ���ݻ���ѹ�����ݡ�
   }

   Expand:
   {
      ExpandΪѹ���ṹ���ܹ��ĸ�nibble(����ֽ�)
      ǰ����nibbleΪ����Slide�е�ƫ�ƣ���һ��nibbleΪ��ƫ�ƶ�Ӧ�ֽ��ظ��Ĵ���-2��
   }

   Block��
   {
      Block��Token���壬���Token��N��λΪ1��BlockΪԭʼ�ֽڣ����Ϊ0��ΪExpand
      Block��С��ȷ�������Ϊ16��ȫ��ΪExpand����СΪ8��ȫ��Ϊԭʼ�ֽ�Ҳ���ǲ�ѹ����
   }

}


LZSS Data:
79 FF 00 1F 01 2A 00 00 FF DF 02 20

Token:
��ͷ��79д�ɶ����ƣ�С����
{1 0 0 1} {1 1 1 0}
   0x9       0x7

Block:
{1    0       0    1} {1 1  1     0}
FF {00 1F} {01 2A} 00 00 FF DF {02 20}
1     0       0    1  1  1  1     0
FF {00 1F} {01 2A} 00 00 FF DF {02 20}

Token��Ӧ��Block�У������1��Ϊԭʼ���ݣ������0��Ϊѹ������
Expand:{00 1F} ѹ������ǰ����nibble������ֽڣ�ΪSlide��ƫ��
��һ��nibbleΪ���ȣ���Ҫ�ظ�������λ��Slide[001]����Ϊ F+2

0x79ÿ�������ƶ�1bit����00000001 and���㣬�� ((Token >>= 1) & 0x1) ? True:False �ж��Ƿ���Ҫչ��

�Ը����ݽ�����
��һ��Ϊ 1 ��չ��
Slide
00 FF
Decode
FF

�ڶ���Ϊ 0 Ҫչ����ƫ��Ϊ001��չ����СΪF+2 = 11
Slide
0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  
00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF 
Decode
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF 

������Ϊ 0 Ҫչ����ƫ��Ϊ012��չ����СΪA+2 = C
Slide
0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  
00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
Decode
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF

����-�߸�Ϊ 1 ��չ��
Slide
0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  
00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00
00 FF DF
Decode
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 00
FF DF

�ڰ˸�Ϊ 0 Ҫչ����ƫ��022��չ����СΪ0+2 = 2
Slide
0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  
00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00
00 FF DF DF DF
Decode
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 00
FF DF DF DF

*/

}