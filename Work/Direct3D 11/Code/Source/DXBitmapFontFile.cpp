#include "DXStdafx.h"
#include "DXFileSystem.h"
#include "DXBitmapFontFile.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXBitmapFontFile::CDXBitmapFontFile()	:	m_pGlyphData(0),
												m_fReciprocalFontSize(1.0f),
												m_fReciprocalTexWidth(1.0f),
												m_fReciprocalTexHeight(1.0f),
												m_uiNumChars(0)
	{
	}


	CDXBitmapFontFile::~CDXBitmapFontFile()
	{
		DX_SAFE_DELETE(m_pGlyphData);
	}


	HRESULT CDXBitmapFontFile::LoadFont(WCHAR* szFileName)
	{
		HRESULT hr = S_OK;

		CDXFileSystem* pFileSystem = CDXFileSystem::GetInstance();

		// The read file.
		const char* pFontData;
		// The read file size.
		size_t	uiFontSize;

		// Read the file.
		DX_V(pFileSystem->ReadFile(szFileName, (void**)&pFontData, &uiFontSize));
		if (SUCCEEDED(hr))	// Success?
		{
			// Get the file header.
			UINT32* pHeader = (UINT32*)pFontData;

			// Header matches? File valid if it does.
			if (*pHeader == DX_BITMAPFONTFILE_HEADER)
			{
				// Header is 4 bytes. So we move ahead 4 bytes.
				pFontData += sizeof(UINT32);

				// Next byte must represent the file info block.
				if (*pFontData == DX_BITMAPFONTFILE_BLOCK_TYPE_INFO)
				{
					// Good? continue.
					pFontData++;

					// The info block size.
					UINT32* pInfoBlockSize = (UINT32*)pFontData;

					// Move ahead.
					pFontData += sizeof(UINT32);
					// The file info.
					Info* pInfo = (Info*)pFontData;
					// We need only the font size from this struct.
					m_fReciprocalFontSize = 1.0f / (float)pInfo->fontSize;

					// Move ahead.
					pFontData += *pInfoBlockSize;

					// Next byte must represent the common block.
					if (*pFontData == DX_BITMAPFONTFILE_BLOCK_TYPE_COMMON)
					{
						pFontData++;

						// The block size.
						UINT32* pCommonBlockSize = (UINT32*)pFontData;
						pFontData += sizeof(UINT32);

						// We require the texture dimensions from this block.
						Common* pCommon = (Common*)pFontData;
						m_fReciprocalTexWidth = 1.0f / (float)pCommon->scaleW;
						m_fReciprocalTexHeight = 1.0f / (float)pCommon->scaleH;

						// Go ahead.
						pFontData += *pCommonBlockSize;
						// Next block must be the pages block.
						if (*pFontData == DX_BITMAPFONTFILE_BLOCK_TYPE_PAGES)
						{
							pFontData++;

							UINT32* pPagesBlockSize = (UINT32*)pFontData;
							pFontData += sizeof(UINT32);
							// We don't need anything from this block.
							pFontData += *pPagesBlockSize;

							// At last we are here! The characters data.
							if (*pFontData == DX_BITMAPFONTFILE_BLOCK_TYPE_CHARS)
							{
								pFontData++;
								// The block size.
								UINT32* pCharsBlockSize = (UINT32*)pFontData;
								// Number of chars in the file.
								UINT32 uiNumChars = (*pCharsBlockSize) / sizeof(Char);
								pFontData += sizeof(UINT32);

								// The characters data array!
								const Char* pChar = (Char*)pFontData;
								const Char* pLastChar = pChar + uiNumChars - 1;

								// Allocate memory for our characters information.
								m_pGlyphData = new DXBitmapFontGlyphData[pLastChar->id + 1];
								
								// Iterator through the characters.
								for (UINT i=0; i<uiNumChars; i++)
								{
									UINT index = pChar->id;

									// Calculate the texture coords.
									float fLeft = (float)pChar->x * m_fReciprocalTexWidth;
									float fTop = (float)pChar->y * m_fReciprocalTexHeight;
									float fRight = fLeft + ( (float)(pChar->width) * m_fReciprocalTexWidth );
									float fBottom = fTop + ( (float)(pChar->height) * m_fReciprocalTexHeight );
									
									// Calculate the glyph offsets.
									float fXOffset = (pChar->xOffset != -1) ? (float)(pChar->xOffset) : 0.0f;
									float fYOffset = (pChar->yOffset != -1) ? (float)(pChar->yOffset) : 0.0f;
									
									m_pGlyphData[index].rectTex = DXRect(fLeft, fTop, fRight, fBottom);
									m_pGlyphData[index].fOffsetX = fXOffset * m_fReciprocalTexWidth;
									m_pGlyphData[index].fOffsetY = (fYOffset - 8.0f) * m_fReciprocalTexHeight;
									m_pGlyphData[index].fAdvanceX = (float)(pChar->xAdvance) * m_fReciprocalTexWidth;

									pChar++;
								}
							}
							else
							{
								DX_DEBUG_OUTPUT1(L"ERROR!!! Error reading chars block: %s.", szFileName);
								hr = E_FAIL;
							}
						}
						else
						{
							DX_DEBUG_OUTPUT1(L"ERROR!!! Error reading pages block: %s.", szFileName);
							hr = E_FAIL;
						}
					}
					else
					{
						DX_DEBUG_OUTPUT1(L"\nERROR!!!: Error reading common block: %s.", szFileName);
						hr = E_FAIL;
					}
				}
				else
				{
					DX_DEBUG_OUTPUT1(L"ERROR!!! Error reading info block: %s.", szFileName);
					hr = E_FAIL;
				}
			}
			else
			{
				DX_DEBUG_OUTPUT1(L"ERROR!!! Invalid file: %s.", szFileName);
				hr = E_FAIL;
			}
		}
		else
		{
			DX_DEBUG_OUTPUT1(L"ERROR!!! Unable to read file: %s.\n", szFileName);
			hr = E_FAIL;
		}


		if (FAILED(hr))
		{
			DX_SAFE_DELETE(m_pGlyphData);
		}

		pFileSystem->DecreaseFileUsage(szFileName);
		pFileSystem->FreeFile(szFileName);

		return hr;
	}

} // DXFramework namespace end