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

#include "Model.h"

#include "stdio.h"

#include "glib.h"
#include "gmodule.h"

#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

G_BEGIN_DECLS

enum MODEL_SOURCE_FILE_PROPERTIES
{
    MODEL_SOURCE_FILE_PATH_PROP = 1,
    MODEL_SOURCE_FILE_TYPE_PROP,

    MODEL_SOURCE_FILE_PROP_COUNT
};

///Constructor for SourceFile type.
///location - file location.
ModelSourceFile*
model_source_file_new(GString* location);

///Deserialization function.
ModelSourceFile*
model_source_file_new_from_xml(xmlNodePtr node,
                               GError** error);

///Serialization function.
void
model_source_file_write_xml(ModelSourceFile* file, 
                            xmlTextWriterPtr writer,
                            GError** error);

///Available source file types.
enum ModelSourceFileTypes
{
    CODE,
    HEADER,
    VIEW,
    RESOURCE,

    ModelSourceFileTypesCOUNT
};

///Getter for File type property(as ModelSourceFileTypes enum).
int
model_source_file_get_file_type(ModelSourceFile* file);

///Equilty function for two SourceFile instances.
gboolean
model_source_file_equals(const void* this, const void* that);

///Getter for Path property.
GString*
model_source_file_get_path(ModelSourceFile* this);

///Getter for Dependencies property.
GPtrArray*
model_source_file_get_deps(ModelSourceFile* this);

G_END_DECLS

#endif