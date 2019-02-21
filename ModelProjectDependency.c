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


#include "ModelProjectDependency.h"

#include "Helper.h"
#include "string.h"

struct _ModelProjectDependency
{
    GObject parent_object;

    GString* _representation;
    gint _type;
};

G_DEFINE_TYPE(ModelProjectDependency, model_project_dependency, G_TYPE_OBJECT);

static void
model_project_dependency_class_init(ModelProjectDependencyClass* class)
{}

static void
model_project_dependency_init(ModelProjectDependency* this)
{}

ModelProjectDependency*
model_project_dependency_new(GString* representation, gint type)
{   
    ModelProjectDependency* this = g_object_new(MODEL_TYPE_PROJECT_DEPENDENCY, NULL);

    this->_representation = representation;
    this->_type = type;

    return this;
}

ModelProjectDependency*
model_project_dependency_new_from_xml(xmlNodePtr node)
{
    xmlNodePtr dep = node->children;
    ModelProjectDependency* this = g_object_new(MODEL_TYPE_PROJECT_DEPENDENCY, NULL);

    while(dep != NULL)
    {
        if(strcmp(dep->name, "Representation") == 0)
        {
            this->_representation = g_string_new(xmlNodeGetContent(dep));
        }
        else if(strcmp(dep->name, "Type") == 0)
        {
            this->_type = atoi(xmlNodeGetContent(dep));
            g_assert(this->_type < LAST_DEP_TYPE);
        }

        dep = dep->next;
    }

    return this;
}

void
model_project_dependency_write_xml(ModelProjectDependency* this, xmlTextWriterPtr writer)
{
    g_assert(this);
    g_assert(writer);

    int rc;

    rc = xmlTextWriterStartElement(writer, BAD_CAST "Dependency");
    g_assert(rc >= 0);

    char num[16];
    sprintf(num, "%d", this->_type);

    xmlTextWriterWriteElement(writer, "Representation", this->_representation->str);
    xmlTextWriterWriteElement(writer, "Type", num);

    rc = xmlTextWriterEndElement(writer);
    g_assert(rc >= 0);
}

GString*
model_project_dependency_get_representation(ModelProjectDependency* this)
{
    g_assert(this);

    return this->_representation;
}

GString*
model_project_dependency_get_includes(ModelProjectDependency* this)
{
    g_assert(this);
    if(this->_type == SYSTEM_DEP) //в майбутньому розширити для підтримки CBS залежності
    {
        char** args = (char**)malloc(sizeof(char*) * 4);
        args[0] = g_strdup("pkgconf");
        args[1] = g_strdup("--cflags");
        args[2] = this->_representation->str;
        args[3] = (char*)NULL;

        GString* out = run_tool("/usr/bin/pkgconf", args); //в мабутньому варто перевіряти чи є pkg-config взагалі

        //додати перевірку на правильність рядка

        g_free(args);

        return out;
    }

    return NULL;
}

GString*
model_project_dependency_get_links(ModelProjectDependency* this)
{
    g_assert(this);

    if(this->_type == SYSTEM_DEP)
    {
        char** args = (char**)malloc(sizeof(char*) * 4);
        args[0] = g_strdup("pkgconf");
        args[1] = g_strdup("--libs");
        args[2] = g_strdup(this->_representation->str);
        args[3] = (char*)NULL;

        GString* out = run_tool("/usr/bin/pkgconf", args); 

        for(int i = 0; i<4; i++)
        {
            if(args[i])
                g_free(args[i]);
        }

        g_free(args);

        return out;
    }
    else if(this->_type == EXTERNAL_DYNAMIC_LIB)
    {
        GString* loc = g_string_new("-l");
        loc = g_string_append(loc, this->_representation->str);

        return g_string_append(loc, " ");
    }
    else if(this->_type == CBS_PROJECT)
    {
        GString* projLoc = g_string_new("-L");
        projLoc = g_string_append(projLoc, g_path_get_dirname(this->_representation->str));
        
        return g_string_append(projLoc, "/bin ");
    }

    return NULL;
}

gint
model_project_dependency_get_dependency_type(ModelProjectDependency* this)
{
    g_assert(this);
    
    return this->_type;
}

gboolean
model_project_dependency_equals(const void* obj1, const void* obj2)
{
    ModelProjectDependency* dep1 = (ModelProjectDependency*)obj1;
    ModelProjectDependency* dep2 = (ModelProjectDependency*)obj2;

    if(dep1 != NULL && dep2 != NULL)
    {
        return g_string_equal(dep1->_representation, dep2->_representation) && dep1->_type == dep2->_type;
    }

    return FALSE;
}