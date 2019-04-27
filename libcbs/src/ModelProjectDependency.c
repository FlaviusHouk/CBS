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
#include "ModelProject.h"
#include "ModelProjectManager.h"

#include "Helper.h"
#include "string.h"

struct _ModelProjectDependency
{
    GObject parent_object;

    ModelProject* _owner;

    GString* _representation;
    gint _type;
};

G_DEFINE_TYPE(ModelProjectDependency, model_project_dependency, G_TYPE_OBJECT);

static void
model_project_dependency_dispose(GObject* obj)
{
    ModelProjectDependency* this = MODEL_PROJECT_DEPENDENCY(obj);

    if(this->_owner)
    {
        g_object_unref(this->_owner);
        this->_owner = NULL;
    }

    if(this->_representation)
    {
        g_string_free(this->_representation, TRUE);
        this->_representation = NULL;
    }

    G_OBJECT_CLASS(model_project_dependency_parent_class)->dispose(obj);
}

static void
model_project_dependency_finalize(GObject* obj)
{
    G_OBJECT_CLASS(model_project_dependency_parent_class)->finalize(obj);
}

static void
model_project_dependency_class_init(ModelProjectDependencyClass* class)
{
    GObjectClass* objectClass = G_OBJECT_CLASS(class);

    objectClass->dispose = model_project_dependency_dispose;
    objectClass->finalize = model_project_dependency_finalize;
}

static void
model_project_dependency_init(ModelProjectDependency* this)
{
    this->_owner = NULL;
}

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
model_project_dependency_get_includes(ModelProjectDependency* this,
                                      GError ** error)
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
    else if(this->_type == CBS_PROJECT)
    {
        GString* resolvedPath = model_project_resolve_path(this->_owner, this->_representation);

        ModelProject* dep = NULL;
        if(!model_project_load_or_create_project(resolvedPath, &dep))
        {
            g_set_error(error,
                        g_type_qname(MODEL_TYPE_PROJECT_DEPENDENCY),
                        MODEL_PROJECT_DEPENDENCY_FAILED_TO_PROCESS,
                        "Cannot locate project %s.\n",
                        resolvedPath->str);

            return NULL;
        }

        GString* loc = g_string_new(g_path_get_dirname(model_project_get_location(dep)->str));

        loc = g_string_prepend(loc, g_strdup("-I"));

        return g_string_append(loc, g_strdup("/headers  "));
    }

    return NULL;
}

GString*
model_project_dependency_get_links(ModelProjectDependency* this,
                                   GError** error)
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
        gchar* libName = g_path_get_basename(this->_representation->str);
        gchar* libPath = g_path_get_dirname(this->_representation->str);
        gchar* absPath = g_path_get_absolute(libPath);

        GString* link = g_string_new("-L");

        g_string_append_printf(link, "%s -l%s -Wl,-rpath=%s  ", libPath, libName, absPath);

        return link;
    }
    else if(this->_type == CBS_PROJECT)
    {
        GString* resovledPath = model_project_resolve_path(this->_owner, this->_representation);

        ModelProject* dep = NULL;
        if(!model_project_load_or_create_project(resovledPath, &dep))
        {
            g_set_error(error,
                        g_type_qname(MODEL_TYPE_PROJECT_DEPENDENCY),
                        MODEL_PROJECT_DEPENDENCY_FAILED_TO_PROCESS,
                        "Cannot locate project %s./n",
                        resovledPath->str);

            return NULL;
        }

        ModelProjectConfiguration* conf = model_project_get_build_config(dep, NULL);

        gint outputType = model_project_configuration_get_output_type(conf);
        g_assert(outputType != ELF);

        if(outputType == DYNAMIC_LIB)
        {
            ModelProjectManager* manager = model_project_manager_new();

            GError* innerError = NULL;
            model_project_manager_build_project(manager, dep, NULL, &innerError);
            if(innerError != NULL)
            {
                g_set_error(error,
                            g_type_qname(MODEL_TYPE_PROJECT_DEPENDENCY),
                            MODEL_PROJECT_DEPENDENCY_FAILED_TO_PROCESS,
                            "Failed to builed dependency.\n%s",
                            g_strdup(innerError->message));
                
                g_clear_error(&innerError);

                return NULL;
            }

            g_object_unref(manager);

            gchar* libName = g_path_get_basename(this->_representation->str);

            if(g_str_has_prefix(libName, "lib") && g_str_has_suffix(libName, ".cpd"))
            {
                gchar* linkName = g_str_substr(libName, 3, strlen(libName) - 7);
                g_free(libName);

                libName = linkName;
            }
            else if(g_str_has_suffix(libName, ".cpd"))
            {
                gchar* linkName = g_str_substr(libName, 0, strlen(libName) - 4);
                g_free(libName);

                libName = linkName;
            }

            gchar* libPath = g_path_get_dirname(resovledPath->str);

            GString* link = g_string_new("-L");

            gchar* absPath = g_path_get_absolute(libPath);

            g_string_append_printf(link, "%s/bin -l%s -Wl,-rpath=%s/bin  ", libPath, libName, absPath);

            return link;
        }

        g_object_unref(dep);
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

    if(dep1 != NULL && dep2 != NULL && 
       dep1->_representation != NULL && dep2->_representation != NULL)
    {
        return g_string_equal(dep1->_representation, dep2->_representation) && dep1->_type == dep2->_type;
    }

    return FALSE;
}

ModelProject*
model_project_dependency_get_owner(ModelProjectDependency* this)
{
    g_assert(this);

    if(this->_owner)
        g_object_ref(this->_owner);

    return this->_owner;
}

void
model_project_dependency_set_owner(ModelProjectDependency* this, ModelProject* owner)
{
    g_assert(this);

    if(this->_owner)
        g_object_unref(this->_owner);

    if(owner)
        g_object_ref(owner);

    this->_owner = owner;
}

