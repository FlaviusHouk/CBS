#ifndef _MODEL_SOURCE_FILE_H_
#define _MODEL_SOURCE_FILE_H_

#include "stdio.h"
#include "glib-object.h"

#include "glib.h"
#include "gmodule.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

G_BEGIN_DECLS

#define MODEL_TYPE_SOURCE_FILE model_source_file_get_type()

G_DECLARE_FINAL_TYPE(ModelSourceFile, model_source_file, MODEL, SOURCE_FILE, GObject);

G_END_DECLS

ModelSourceFile*
model_source_file_new(GString* location);

ModelSourceFile*
model_source_file_new_from_xml(xmlNodePtr node);

void
model_source_file_write_xml(ModelSourceFile* file, xmlTextWriterPtr writer);

enum ModelSourceFileTypes
{
    CODE,
    HEADER,
    VIEW,
    RESOURCE
};

int
model_source_file_get_file_type(ModelSourceFile* file);

gboolean
model_source_file_equals(const void* this, const void* that);

GString*
model_source_file_get_path(ModelSourceFile* this);

#endif