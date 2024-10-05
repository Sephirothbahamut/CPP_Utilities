#include "text.h"

#include <string>
#include <codecvt>
#include <unordered_map>

#include "colour.h"
#include "../compilation/OS.h"
#include "../math/transform2.h"
#include "../math/geometry/shape/mixed.h"
#include "../math/geometry/interactions/mixed.h"

//TODO remove Utilities_MS dependency
#include "../../../../../CPP_Utilities_MS/include/utils/MS/graphics/d2d.h"

#ifndef utils_compilation_os_windows
#error Only windows supported so far because text.cpp relies on DirectWrite. 
//DirectWriteCore should be cross platform but I couldn't figure out how to include it properly and I don't really use linux so that's low priority.
//TODO user DirectWriteCore and remove windows only-ness
#endif

#include <Windows.h>
#include <dcomp.h>
#include <d2d1_3.h>
#include <d3d11_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <windows.ui.composition.interop.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")

namespace utils::graphics::text
	{
	struct geometry_sink : ID2D1GeometrySink
		{
		public:
			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);


			IFACEMETHOD_(void, BeginFigure)(D2D1_POINT_2F startPoint, D2D1_FIGURE_BEGIN figureBegin);
			IFACEMETHOD_(void, EndFigure)(D2D1_FIGURE_END figureEnd);

			IFACEMETHOD_(void, AddLine)(D2D1_POINT_2F point);

			IFACEMETHOD_(void, AddBezier)(_In_ CONST D2D1_BEZIER_SEGMENT* bezier);
			IFACEMETHOD_(void, AddBezier)(_In_ CONST D2D1_BEZIER_SEGMENT& bezier);

			IFACEMETHOD_(void, AddQuadraticBezier)(_In_ CONST D2D1_QUADRATIC_BEZIER_SEGMENT* bezier);
			IFACEMETHOD_(void, AddQuadraticBezier)(_In_ CONST D2D1_QUADRATIC_BEZIER_SEGMENT& bezier);

			IFACEMETHOD_(void, AddArc)(_In_ CONST D2D1_ARC_SEGMENT* arc);

			IFACEMETHOD_(void, AddBeziers)(_In_ CONST D2D1_BEZIER_SEGMENT* beziers, UINT32 beziersCount);
			IFACEMETHOD_(void, AddLines)(_In_ CONST D2D1_POINT_2F* points, UINT32 pointsCount);
			IFACEMETHOD_(void, AddQuadraticBeziers)(_In_ CONST D2D1_QUADRATIC_BEZIER_SEGMENT* beziers, UINT32 beziersCount);

			IFACEMETHOD_(void, SetFillMode)(D2D1_FILL_MODE fillMode);
			IFACEMETHOD_(void, SetSegmentFlags)(D2D1_PATH_SEGMENT vertexFlags);

			IFACEMETHOD_(HRESULT, Close)();

			std::vector<glyph_t> glyphs;

		private:
			glyph_t current_glyph;

			unsigned long cRefCount_{0};
		};

	IFACEMETHODIMP_(void) geometry_sink::SetFillMode    (D2D1_FILL_MODE    fillMode   ) {}
	IFACEMETHODIMP_(void) geometry_sink::SetSegmentFlags(D2D1_PATH_SEGMENT vertexFlags) {}
	IFACEMETHODIMP_(HRESULT) geometry_sink::Close()
		{
		return S_OK;
		}

	IFACEMETHODIMP_(void) geometry_sink::BeginFigure(D2D1_POINT_2F startPoint, D2D1_FIGURE_BEGIN figureBegin)
		{
		current_glyph.clear({startPoint.x, startPoint.y});
		}
	IFACEMETHODIMP_(void) geometry_sink::EndFigure(D2D1_FIGURE_END figureEnd)
		{
		current_glyph.close();
		glyphs.emplace_back(std::move(current_glyph));
		}

	IFACEMETHODIMP_(void) geometry_sink::AddBeziers(_In_ CONST D2D1_BEZIER_SEGMENT* beziers, UINT32 beziersCount)
		{
		for (size_t i{0}; i < beziersCount; i++) { AddBezier(beziers[i]); }
		}
	IFACEMETHODIMP_(void) geometry_sink::AddLines(_In_ CONST D2D1_POINT_2F* points, UINT32 pointsCount)
		{
		for (size_t i{0}; i < pointsCount; i++) { AddLine(points[i]); }
		}
	IFACEMETHODIMP_(void) geometry_sink::AddQuadraticBeziers(_In_ CONST D2D1_QUADRATIC_BEZIER_SEGMENT* beziers, UINT32 beziersCount)
		{
		for (size_t i{0}; i < beziersCount; i++) { AddQuadraticBezier(beziers[i]); }
		}

	IFACEMETHODIMP_(void) geometry_sink::AddLine(D2D1_POINT_2F point)
		{
		current_glyph.add_segment({point.x, point.y});
		}

	IFACEMETHODIMP_(void) geometry_sink::AddBezier(_In_ CONST D2D1_BEZIER_SEGMENT* bezier) { AddBezier(*bezier); }
	IFACEMETHODIMP_(void) geometry_sink::AddBezier(_In_ CONST D2D1_BEZIER_SEGMENT& bezier)
		{
		current_glyph.add_bezier_4pt
			(
			utils::math::vec2f{bezier.point1.x, bezier.point1.y},
			utils::math::vec2f{bezier.point2.x, bezier.point2.y},
			utils::math::vec2f{bezier.point3.x, bezier.point3.y}
			);

		//current_glyph.add_bezier_3pt
		//	(
		//	(utils::math::vec2f{bezier.point1.x, bezier.point1.y} + utils::math::vec2f{bezier.point2.x, bezier.point2.y}) / 2.f,
		//	utils::math::vec2f{bezier.point3.x, bezier.point3.y}
		//	);
		}

	IFACEMETHODIMP_(void) geometry_sink::AddQuadraticBezier(_In_ CONST D2D1_QUADRATIC_BEZIER_SEGMENT* bezier) { AddQuadraticBezier(*bezier); }
	IFACEMETHODIMP_(void) geometry_sink::AddQuadraticBezier(_In_ CONST D2D1_QUADRATIC_BEZIER_SEGMENT& bezier)
		{
		std::initializer_list points
			{
			utils::math::vec2f{bezier.point1.x, bezier.point1.y},
			utils::math::vec2f{bezier.point2.x, bezier.point2.y}
			};

		current_glyph.add_bezier_3pt(points);
		}
	IFACEMETHODIMP_(void) geometry_sink::AddArc(_In_ CONST D2D1_ARC_SEGMENT* arc)
		{
		std::cout << "arc not supported\n";
		}

	struct glyphs_converter : IDWriteTextRenderer
		{
		public:
			utils::math::transform2 transform;

			IFACEMETHOD(IsPixelSnappingDisabled)
				(
				__maybenull void* clientDrawingContext,
				__out BOOL* isDisabled
				);

			IFACEMETHOD(GetCurrentTransform)
				(
				__maybenull void* clientDrawingContext,
				__out DWRITE_MATRIX* transform
				);

			IFACEMETHOD(DrawGlyphRun)
				(
				__maybenull void* clientDrawingContext,
				FLOAT baselineOriginX,
				FLOAT baselineOriginY,
				DWRITE_MEASURING_MODE measuringMode,
				__in DWRITE_GLYPH_RUN const* glyphRun,
				__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
				IUnknown* clientDrawingEffect
				);

			IFACEMETHOD(GetPixelsPerDip)
				(
				__maybenull void* clientDrawingContext,
				__out FLOAT* pixelsPerDip
				);

			IFACEMETHOD(DrawUnderline)
				(
				__maybenull void* clientDrawingContext,
				FLOAT baselineOriginX,
				FLOAT baselineOriginY,
				__in DWRITE_UNDERLINE const* underline,
				IUnknown* clientDrawingEffect
				);

			IFACEMETHOD(DrawStrikethrough)
				(
				__maybenull void* clientDrawingContext,
				FLOAT baselineOriginX,
				FLOAT baselineOriginY,
				__in DWRITE_STRIKETHROUGH const* strikethrough,
				IUnknown* clientDrawingEffect
				);

			IFACEMETHOD(DrawInlineObject)
				(
				__maybenull void* clientDrawingContext,
				FLOAT originX,
				FLOAT originY,
				IDWriteInlineObject* inlineObject,
				BOOL isSideways,
				BOOL isRightToLeft,
				IUnknown* clientDrawingEffect
				);

		public:
			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface)
				(
				IID const& riid,
				void** ppvObject
				);

		private:
			unsigned long cRefCount_{0};
		};

	
	IFACEMETHODIMP glyphs_converter::GetPixelsPerDip(__maybenull void* clientDrawingContext, __out FLOAT* pixelsPerDip) { *pixelsPerDip = 1.f; return S_OK; }

	IFACEMETHODIMP glyphs_converter::DrawUnderline(__maybenull void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, __in DWRITE_UNDERLINE const* underline, IUnknown* clientDrawingEffect) { return S_OK; }

	IFACEMETHODIMP glyphs_converter::DrawStrikethrough(__maybenull void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, __in DWRITE_STRIKETHROUGH const* strikethrough, IUnknown* clientDrawingEffect) { return S_OK; }

	IFACEMETHODIMP glyphs_converter::DrawInlineObject(__maybenull void* clientDrawingContext, FLOAT originX, FLOAT originY, IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft, IUnknown* clientDrawingEffect) { return S_OK; }




	IFACEMETHODIMP glyphs_converter::IsPixelSnappingDisabled(__maybenull void* clientDrawingContext, __out BOOL* isDisabled)
		{
		*isDisabled = FALSE;
		return S_OK;
		}



	IFACEMETHODIMP glyphs_converter::GetCurrentTransform(__maybenull void* clientDrawingContext, __out DWRITE_MATRIX* transform)
		{
		transform->dx = this->transform.translation.x();
		transform->dy = this->transform.translation.y();
		//TODO understand other factors for rotation/scaling
		return S_OK;
		}

	void print(const std::array<float, 20>& arr)
		{
		for (size_t i{0}; i < arr.size(); i++)
			{
			const float& f{arr[i]};
			std::cout << f << " ";
			}
		std::cout << std::endl;
		}


	IFACEMETHODIMP glyphs_converter::DrawGlyphRun
		(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		__in DWRITE_GLYPH_RUN const* glyphRun,
		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
		IUnknown* clientDrawingEffect
		)
		{
		HRESULT hr = S_OK;
		utils::MS::graphics::d2d::factory d2d_factory;
		
		// Create the path geometry.
		ID2D1PathGeometry* pPathGeometry = NULL;
		hr = d2d_factory->CreatePathGeometry(&pPathGeometry);
		
		// Write to the path geometry using the geometry sink.
		ID2D1GeometrySink* pSink = NULL;

		if (SUCCEEDED(hr))
			{
			hr = pPathGeometry->Open(&pSink);
			}
		else
			{
			throw std::runtime_error{"idk directx stuff"};
			}
		
		// Get the glyph run outline geometries back from DirectWrite and place them within the
		// geometry sink.
		if (SUCCEEDED(hr))
			{
			hr = glyphRun->fontFace->GetGlyphRunOutline
				(
				glyphRun->fontEmSize,
				glyphRun->glyphIndices,
				glyphRun->glyphAdvances,
				glyphRun->glyphOffsets,
				glyphRun->glyphCount,
				glyphRun->isSideways,
				glyphRun->bidiLevel % 2,
				pSink
				);
			}
		else
			{
			throw std::runtime_error{"idk directx stuff"};
			}
		
			// Close the geometry sink
		if (SUCCEEDED(hr))
			{
			hr = pSink->Close();
			}
		else
			{
			throw std::runtime_error{"idk directx stuff"};
			}
		
			// Initialize a matrix to translate the origin of the glyph run.
		D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
			1.0f, 0.0f,
			0.0f, 1.0f,
			baselineOriginX, baselineOriginY
		);
		
		// Create the transformed geometry
		//ID2D1TransformedGeometry* pTransformedGeometry = NULL;
		//if (SUCCEEDED(hr))
		//	{
		//	hr = d2d_factory->CreateTransformedGeometry(pPathGeometry, &matrix, &pTransformedGeometry);
		//	}
		//else
		//	{
		//	throw std::runtime_error{"idk directx stuff"};
		//	}
		//Cannot use transformed geometry->Stream ???
		
		geometry_sink geometry_sink;
		pPathGeometry->Stream(&geometry_sink);
		
		//Since we're not using transformed geometry->Stream we need to transform manually
		//TODO understand why
		
		struct teststruct { int i{0}; float f{1.f}; double d{2.}; };
		
		teststruct testinstance{.f{2.f}};
		utils::math::transform2 testtransform{.translation{1.f, 1.f}, .rotation{0.f}, .scaling{1.f}};
		
		for (auto& glyph : geometry_sink.glyphs)
			{
			utils::math::transform2 transform{.translation{baselineOriginX, baselineOriginY}}; 
			utils::math::geometry::interactions::transform_self(glyph, transform);
			}
		
		//Emplace because this function may be called multiple times
		
		std::vector<glyph_t>& glyphs{*reinterpret_cast<std::vector<glyph_t>*>(clientDrawingContext)};
		
		if (geometry_sink.glyphs.size())
			{
			glyphs.reserve(glyphs.size() + geometry_sink.glyphs.size());
			std::move(geometry_sink.glyphs.begin(), geometry_sink.glyphs.end(), std::back_inserter(glyphs));
			}
		
		//pTransformedGeometry->Release();
		pPathGeometry       ->Release();
		pSink               ->Release();

		return hr;
		}











	IFACEMETHODIMP_(unsigned long) glyphs_converter::AddRef()
		{
		return InterlockedIncrement(&cRefCount_);
		}

	IFACEMETHODIMP_(unsigned long) glyphs_converter::Release()
		{
		unsigned long newCount = InterlockedDecrement(&cRefCount_);
		if (newCount == 0)
			{
			delete this;
			return 0;
			}

		return newCount;
		}

	IFACEMETHODIMP glyphs_converter::QueryInterface(IID const& riid, void** ppvObject)
		{
		if (__uuidof(IDWriteTextRenderer) == riid) { *ppvObject = this; }
		else if (__uuidof(IDWritePixelSnapping) == riid) { *ppvObject = this; }
		else if (__uuidof(IUnknown) == riid) { *ppvObject = this; }
		else { *ppvObject = NULL; return E_FAIL; }

		this->AddRef();

		return S_OK;
		}

	IFACEMETHODIMP_(unsigned long) geometry_sink::AddRef()
		{
		return InterlockedIncrement(&cRefCount_);
		}

	IFACEMETHODIMP_(unsigned long) geometry_sink::Release()
		{
		unsigned long newCount = InterlockedDecrement(&cRefCount_);
		if (newCount == 0)
			{
			delete this;
			return 0;
			}

		return newCount;
		}

	IFACEMETHODIMP geometry_sink::QueryInterface(IID const& riid, void** ppvObject)
		{
		if (__uuidof(ID2D1GeometrySink) == riid) { *ppvObject = this; }
		else if (__uuidof(IUnknown) == riid) { *ppvObject = this; }
		else { *ppvObject = NULL; return E_FAIL; }

		this->AddRef();

		return S_OK;
		}

	std::wstring widen(const std::string& in)
		{//https://stackoverflow.com/questions/14184709/is-this-code-safe-using-wstring-with-multibytetowidechar
		std::wstring out{};

		if (in.length() > 0)
			{
			// Calculate target buffer size (not including the zero terminator).
			//CP_UTF8 fails cause reasons i guess
			int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in.c_str(), in.size(), NULL, 0);
			if (len == 0)
				{
				throw std::runtime_error("Invalid character sequence.");
				}

			out.resize(len);
			// No error checking. We already know, that the conversion will succeed.
			MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in.c_str(), in.size(), out.data(), out.size());
			}

		return out;
		}

	std::vector<glyph_t> glyphs_from_string(const std::string& text, const std::wstring& font)
		{
		//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> string_converter;
		//std::wstring wide{string_converter.from_bytes(text)};

		std::wstring wide{widen(text)};

		namespace dw = utils::MS::graphics::dw;
		dw::factory dw_factory;
		dw::text_format text_format{dw_factory, dw::text_format::create_info
			{
			.name{font},
			.size{32.f}
			}};
		
		D2D1_RECT_F layoutRect{0.f, 0.f, 1024.f, 500.f};

		dw::text_layout text_layout{dw_factory, wide, text_format, utils::math::vec2u32{uint32_t{2048}, uint32_t{2048}}};

		std::vector<glyph_t> glyphs;

		glyphs_converter converter;
		text_layout->Draw(&glyphs, &converter, 0.f, 0.f);


		return glyphs;
		}
	}
