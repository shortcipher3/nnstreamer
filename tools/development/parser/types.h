/* SPDX-License-Identifier: LGPL-2.1-only */
/**
 * This is imported from GStreamer and altered to parse GST-Pipeline
 *
 * @file  types.h
 * @date  12 Nov 2020
 * @brief GST-Parser's types.h modified for gst2pbtxt parser (nnstreamer parser)
 * @see https://github.com/nnstreamer/nnstreamer
 * @author  MyungJoo Ham <myungjoo.ham@samsung.com>
 * @bug No known bugs except for NYI items
 */
#ifndef __GST_PARSE_TYPES_H__
#define __GST_PARSE_TYPES_H__

#include <glib-object.h>

/** Define GST macro's substitutions */
#define __GST_BIN(e) (((e)->id == oTI_GstBin && (e)->specialType == 0) ? \
  (e) : NULL)
#define __GST_IS_BIN(e) ((e)->id == oTI_GstBin)
#define __GST_BIN_CAST(e) (e)

typedef enum {
  eST_normal = 0,
  eST_URI_SINK,
  eST_URI_SRC,
} elementSpecialType;

typedef enum {
  oTI_Element = 0,
  oTI_GstBin,
} objectTypeId;

typedef struct _chain_t chain_t;

/** @brief Simplified GST-Element */
typedef struct {
  objectTypeId id;
  elementSpecialType specialType;

  gchar *element;
  gchar *name;
  GSList *properties; /**< List of key-value pairs (_Property), added for gst-pbtxt, except for name=.... */
  int refcount;

  union {
    GSList *elements; /**< _GstBin type uses this */
  };
} _Element;

extern _Element *
nnstparser_element_make (const gchar *element, const gchar *name);

/** @brief Simplified GObject Property for GST Element */
typedef struct {
  gchar *name;
  gchar *value;
} _Property;

/** @brief pipeline element/pad reference */
typedef struct {
  _Element *element;
  gchar *name;
  GSList *pads; /**< Unlike original, it's list of string pad-names */
} reference_t;

/** @brief pad-to-pad linkage relation */
typedef struct {
  reference_t src;
  reference_t sink;
  gchar *caps; /**< In this app, caps is generally unidentifiable. simply copy the caps string! */
  gboolean all_pads;
} link_t;

/** @brief Chain of elements */
struct _chain_t {
  GSList *elements; /**< Originally its data is "GstElement". It's now _Element */
  reference_t first;
  reference_t last;
};

extern _Element *
nnstparser_gstbin_make (const gchar * element, const gchar * name);


/**
 * @brief A dummy created for gst2pbtxt
 */
typedef struct {
  GList *missing_elements;
} _ParseContext;

/**
 * @brief A dummy created for gst2pbtxt
 */
typedef enum
{
  __PARSE_FLAG_NONE = 0,
  __PARSE_FLAG_FATAL_ERRORS = (1 << 0),
  __PARSE_FLAG_NO_SINGLE_ELEMENT_BINS = (1 << 1),
  __PARSE_FLAG_PLACE_IN_BIN = (1 << 2)
} _ParseFlags;

/**
 * @brief A copy of GstURIType from gstreamer/gsturi
 */
typedef enum {
  GST_URI_UNKNOWN,
  GST_URI_SINK,
  GST_URI_SRC
} _URIType;

/**
 * @brief A dummy copy of gst_element_make_from_uri from gstreamer/gsturi
 */
extern _Element *
nnstparser_element_from_uri (const _URIType type, const gchar *uri,
    const gchar * elementname, void **error);

/** @brief gst_object_unref for psuedo element */
extern _Element *
nnstparser_element_unref (_Element * element);

/** @brief gst_object_ref for psuedo element */
extern void
nnstparser_element_ref (_Element * element);

typedef struct _graph_t graph_t;
/** @brief The pipeline graph */
struct _graph_t {
  chain_t *chain; /* links are supposed to be done now */
  GSList *links;
  GError **error;
  _ParseContext *ctx; /* may be NULL */
  _ParseFlags flags;
};

/** @brief GST Parser's internal function imported */
static inline void
gst_parse_unescape (gchar *str)
{
  gchar *walk;
  gboolean in_quotes;

  g_return_if_fail (str != NULL);

  walk = str;
  in_quotes = FALSE;

  GST_DEBUG ("unescaping %s", str);

  while (*walk) {
    if (*walk == '\\' && !in_quotes) {
      walk++;
      /* make sure we don't read beyond the end of the string */
      if (*walk == '\0')
        break;
    } else if (*walk == '"' && (!in_quotes || (in_quotes
                && (*(walk - 1) != '\\')))) {
      /** don't unescape inside quotes and don't switch
       * state with escaped quoted inside quotes */
      in_quotes = !in_quotes;
    }
    *str = *walk;
    str++;
    walk++;
  }
  *str = '\0';
}

GQuark gst2pbtxt_parse_error_quark (void);
#define GST2PBTXT_PARSE_ERROR gst2pbtxt_parse_error_quark ()

typedef enum
{
  GST2PBTXT_PARSE_ERROR_SYNTAX,
  GST2PBTXT_PARSE_ERROR_NO_SUCH_ELEMENT,
  GST2PBTXT_PARSE_ERROR_NO_SUCH_PROPERTY,
  GST2PBTXT_PARSE_ERROR_LINK,
  GST2PBTXT_PARSE_ERROR_COULD_NOT_SET_PROPERTY,
  GST2PBTXT_PARSE_ERROR_EMPTY_BIN,
  GST2PBTXT_PARSE_ERROR_EMPTY,
  GST2PBTXT_PARSE_ERROR_DELAYED_LINK
} Gst2PbtxtParseError;


G_GNUC_INTERNAL _Element *priv_gst_parse_launch (const gchar      * str,
                                                   GError          ** err,
                                                   _ParseContext  * ctx,
                                                   _ParseFlags      flags);

/** @brief Substitutes GST's gst_bin_get_by_name () */
_Element *
nnstparser_bin_get_by_name (_Bin * bin, const gchar * name);

/** @brief Substitutes GST's gst_bin_get_by_name_recurse_up () */
_Element *
nnstparser_bin_get_by_name_recurse_up (_Bin * bin, const gchar * name);

#endif /* __GST_PARSE_TYPES_H__ */
