#include "ModelProjectDependency.h"

struct _ModelProjectDependency
{
    GObject parent_object;
};

G_DEFINE_TYPE(ModelProjectDependency, model_project_dependency, G_TYPE_OBJECT);

static void
model_project_dependency_class_init(ModelProjectDependencyClass* class)
{}

static void
model_project_dependency_init(ModelProjectDependency* this)
{}