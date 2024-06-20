#pragma once


namespace AdvHDStaticLibrary
{
	namespace WIP_Struct
	{
		/*
		* WIP File
		*	WIPHeader
		*	WIPEntry[WIPHeader.usFrames]
		*	Data
		*/
		struct WIPHeader
		{
			char aSignature[4];         // WIPF
			unsigned short usFrames;    // Count WIP File
			unsigned short usBpp;       // Bits Per Pixel
		};

		struct WIPEntry
		{
			unsigned int uiWidth;
			unsigned int uiHeigh;
			unsigned int uiX;
			unsigned int uiY;
			unsigned int uiZ;
			unsigned int uiSize;
		};
	}
}