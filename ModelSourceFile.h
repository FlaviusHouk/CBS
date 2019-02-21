/**********************************************************************
This file is part of C Build System.

C Build System is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

C Build System is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with C Build System.  If not, see <https://www.gnu.org/licenses/>.
***********************************************************************/


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