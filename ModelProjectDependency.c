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
        char** args = (char**)g_alloca(sizeof(char*) * 4);
        args[0] = g_strdup("pkgconf");
        args[1] = g_strdup("--cflags");
        args[2] = this->_representation->str;
        args[3] = (char*)NULL;

        GString* out = run_tool("/usr/bin/pkgconf", args); //в мабутньому варто перевіряти чи є pkg-config взагалі

        //додати перевірку на правильність рядка

        return out;
    }

    return NULL;
}

gint
model_project_dependency_get_dependency_type(ModelProjectDependency* this)
{
    g_assert(this);
    
    return this->_type;
}