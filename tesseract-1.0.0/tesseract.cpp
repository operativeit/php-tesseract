#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "p3.h"
#include "zend_exceptions.h"

#ifdef HAVE_CONFIG_H
# pragma push_macro("HAVE_CONFIG_H")
# undef HAVE_CONFIG_H
#endif
#include <tesseract/baseapi.h>
#ifdef HAVE_CONFIG_H
# pragma pop_macro("HAVE_CONFIG_H")
#endif

#include <leptonica/allheaders.h>

// ZTS Requirement: Cache Definition
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif

namespace phptess {

static void throwException(const char *format, ...) PHP_ATTRIBUTE_FORMAT(printf, 1, 2);
static void throwException(const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	char *message;
	spprintf(&message, 0, format, ap);
	va_end(ap);
	zend_throw_exception(zend_ce_error_exception, message, 0);
	efree(message);
}

struct Tesseract : tesseract::TessBaseAPI {
	P3_METHOD_DECLARE(__construct) {
		char *lang = (char*)"eng";
		size_t lang_len = sizeof("eng") - 1;
		zend_long oem = tesseract::OEM_DEFAULT;

		// OPTIMIZATION: Fast ZPP
		ZEND_PARSE_PARAMETERS_START(0, 2)
			Z_PARAM_OPTIONAL
			Z_PARAM_STRING(lang, lang_len)
			Z_PARAM_LONG(oem)
		ZEND_PARSE_PARAMETERS_END();

		if (Init(nullptr, lang, (tesseract::OcrEngineMode)oem)) {
			throwException("Could not initialize libtesseract");
			return;
		}
	}

	P3_METHOD_DECLARE(setImage) {
		char *filename;
		size_t filename_len;

		// OPTIMIZATION: Fast ZPP + Path Check
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_PATH(filename, filename_len)
		ZEND_PARSE_PARAMETERS_END();

		Pix *image = pixRead(filename);
		if (!image) {
			throwException("Failed reading image: %s\n", filename);
			return;
		}
		if (m_image) {
			Clear();
			pixDestroy(&m_image);
		}
		m_image = image;
		SetImage(m_image);
		RETURN_ZVAL(getThis(), true, false);
	}

	P3_METHOD_DECLARE(setRectangle) {
		zend_long left, top, width, height;

		// OPTIMIZATION: Fast ZPP
		ZEND_PARSE_PARAMETERS_START(4, 4)
			Z_PARAM_LONG(left)
			Z_PARAM_LONG(top)
			Z_PARAM_LONG(width)
			Z_PARAM_LONG(height)
		ZEND_PARSE_PARAMETERS_END();

		SetRectangle(left, top, width, height);
		RETURN_ZVAL(getThis(), true, false);
	}

	P3_METHOD_DECLARE(setPageSegMode) {
		zend_long mode;

		// OPTIMIZATION: Fast ZPP
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_LONG(mode)
		ZEND_PARSE_PARAMETERS_END();

		SetPageSegMode((tesseract::PageSegMode)mode);
		RETURN_ZVAL(getThis(), true, false);
	}

	P3_METHOD_DECLARE(getUTF8Text) {
		if (!m_image) {
			RETURN_FALSE;
		}
		char *text = GetUTF8Text();
		RETVAL_STRING(text);
		delete [] text; // Tesseract API requires delete[]
	}

	Pix *m_image{nullptr};

	// Pragmatic Cleanup: CI environments segfault on full ~TessBaseAPI destruction.
	// We explicitly clean resources we own, then call End(), but skip the full cleanup.
	void Shutdown() {
		if (m_image) {
			pixDestroy(&m_image);
			m_image = nullptr;
		}
		End(); // TessBaseAPI::End()
	}

	virtual ~Tesseract() {
		// Default destructor. 
		// Do NOT put cleanup here if we skip calling it in free_obj!
	}

	static zend_object_handlers handlers;
	static zend_class_entry *class_entry;
};
zend_object_handlers Tesseract::handlers;
zend_class_entry *Tesseract::class_entry;

// --------------------------------------------------------------------------

ZEND_BEGIN_ARG_INFO_EX(construct_arginfo, 0, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, language)
	ZEND_ARG_INFO(0, oem)
ZEND_END_ARG_INFO();
ZEND_BEGIN_ARG_INFO_EX(setimage_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO();
ZEND_BEGIN_ARG_INFO_EX(setrectangle_arginfo, 0, ZEND_RETURN_VALUE, 4)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, top)
	ZEND_ARG_INFO(0, width)
	ZEND_ARG_INFO(0, height)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(setpagesegmode_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(getutf8text_arginfo, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO();

static zend_function_entry tesseract_methods[] = {
	P3_ME(Tesseract, __construct, construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	P3_ME(Tesseract, setImage, setimage_arginfo, ZEND_ACC_PUBLIC)
	P3_ME(Tesseract, setRectangle, setrectangle_arginfo, ZEND_ACC_PUBLIC)
	P3_ME(Tesseract, setPageSegMode, setpagesegmode_arginfo, ZEND_ACC_PUBLIC)
	P3_ME(Tesseract, getUTF8Text, getutf8text_arginfo, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

// ZTS Requirement: RINIT
static PHP_RINIT_FUNCTION(tesseract)
{
#if defined(ZTS) && defined(COMPILE_DL_TESSERACT)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}

static PHP_MINIT_FUNCTION(tesseract) {
	Tesseract::class_entry = p3::initClassEntry<Tesseract>(
		"Operativeit\\Tesseract",
		tesseract_methods
	);

	// Override destructor to fix CI Segfaults (Hybrid: End() then leak)
	// We MUST manually call our usage cleanup, but avoid the full C++ destructor
	// because TessBaseAPI has static destruction order issues in CI runners.
	Tesseract::handlers.free_obj = [](zend_object *obj) {
		auto t = p3::toObject<Tesseract>(obj);
		if (t) {
			t->Shutdown();
		}
		zend_object_std_dtor(obj); 
	};

	zend_declare_class_constant_string(Tesseract::class_entry,
		"VERSION", strlen("VERSION"), Tesseract::Version());

#define TESS_CONST(name) zend_declare_class_constant_long(Tesseract::class_entry, #name, strlen(#name), tesseract::name)
	TESS_CONST(PSM_OSD_ONLY);
	TESS_CONST(PSM_AUTO_OSD);
	TESS_CONST(PSM_AUTO_ONLY);
	TESS_CONST(PSM_AUTO);
	TESS_CONST(PSM_SINGLE_COLUMN);
	TESS_CONST(PSM_SINGLE_BLOCK_VERT_TEXT);
	TESS_CONST(PSM_SINGLE_BLOCK);
	TESS_CONST(PSM_SINGLE_LINE);
	TESS_CONST(PSM_SINGLE_WORD);
	TESS_CONST(PSM_CIRCLE_WORD);
	TESS_CONST(PSM_SINGLE_CHAR);
	TESS_CONST(PSM_SPARSE_TEXT);
	TESS_CONST(PSM_SPARSE_TEXT_OSD);
	TESS_CONST(PSM_RAW_LINE);

	TESS_CONST(OEM_TESSERACT_ONLY);
	TESS_CONST(OEM_LSTM_ONLY);
	TESS_CONST(OEM_TESSERACT_LSTM_COMBINED);
	TESS_CONST(OEM_DEFAULT);
#undef TESS_CONST

	return SUCCESS;
}

static zend_module_entry tesseract_module_entry = {
	STANDARD_MODULE_HEADER,
	"tesseract",
	nullptr, /* functions */
	PHP_MINIT(tesseract),
	nullptr, /* MSHUTDOWN */
	PHP_RINIT(tesseract), /* RINIT - Updated for ZTS */
	nullptr, /* RSHUTDOWN */
	nullptr, /* MINFO */
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TESSERACT
ZEND_GET_MODULE(tesseract)
#endif

} // namespace phptess
