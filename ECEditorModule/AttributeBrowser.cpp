// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "AttributeBrowser.h"
#include "AttributeInterface.h"
#include "ECAttributeEditor.h"

#include <QtAbstractEditorFactoryBase>
#include <QtTreePropertyBrowser>
#include <QLayout>

#include "MemoryLeakCheck.h"

namespace ECEditor
{
    AttributeBrowser::AttributeBrowser(QWidget *parent): QWidget(parent),
        propertyBrowser_(0)
    {
        InitializeEditor();
    }

    AttributeBrowser::~AttributeBrowser()
    {
    }

    void AttributeBrowser::AddEntityComponents(std::vector<Foundation::ComponentInterfacePtr> entityComponents)
    {
        if(!entityComponents.size())
            return;

        QString name = QString(entityComponents[0]->TypeName().c_str());
        for(uint i = 0; i < entityComponents.size(); i++)
            SelectedEntityComponents_[name].push_back(Foundation::ComponentWeakPtr(entityComponents[i]));
        RefreshAttributeComponents();
    }

    void AttributeBrowser::RefreshAttributeComponents()
    {
        EntityComponentMap::iterator iter = SelectedEntityComponents_.begin();
        while(iter != SelectedEntityComponents_.end())
        {
            for(uint i = 0; i < iter->second.size(); i++)
            {
                if(iter->second[i].expired())
                    continue;
                
                Foundation::ComponentInterfacePtr componentPtr = iter->second[i].lock();
                Foundation::AttributeVector attributes = componentPtr->GetAttributes();
                for(uint j = 0; j < attributes.size(); j++)
                {
                    AddNewAttribute(*attributes[j], componentPtr);
                }
            }
            iter++;
        }
    }

    void AttributeBrowser::ClearBrowser()
    {
        propertyBrowser_->clear();
        while(!SelectedEntityComponents_.empty())
            SelectedEntityComponents_.erase(SelectedEntityComponents_.begin());

        while(!attributes_.empty())
        {
            SAFE_DELETE(attributes_.begin()->second)
            attributes_.erase(attributes_.begin());
        }
    }

    void AttributeBrowser::InitializeEditor()
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        propertyBrowser_ = new QtTreePropertyBrowser(this);
        propertyBrowser_->setResizeMode(QtTreePropertyBrowser::ResizeToContents);
        layout->addWidget(propertyBrowser_);
    }

    void AttributeBrowser::AddNewAttribute(const Foundation::AttributeInterface &attribute, Foundation::ComponentInterfacePtr component)
    {
        AttributeEditorMap::iterator iter = attributes_.find(attribute.GetName());
        if(iter == attributes_.end())
        {
            // if attribute editor is not created for that spesific attribute interface do so.
            ECAttributeEditorBase *newEditor = CreateAttributeEditor(attribute, component);
            if(newEditor)
                attributes_[newEditor->GetAttributeName()] = newEditor;
        }
        else
        {
            // If component contains attribute that has been already created. Add that component to attribute editor.
            iter->second->AddNewComponent(component);
        }
    }

    ECAttributeEditorBase *AttributeBrowser::CreateAttributeEditor(const Foundation::AttributeInterface &attribute, Foundation::ComponentInterfacePtr component)
    {
        ECAttributeEditorBase *attributeEditor = 0;
        // Todo! Organize those dynamic casts in a such order that we first check those attribute types that are most commonly used.
        if(dynamic_cast<const Foundation::Attribute<Real> *>(&attribute))
            attributeEditor = new ECAttributeEditor<Real>(attribute.GetName(), propertyBrowser_, component, this);
        /*else if(dynamic_cast<const Foundation::Attribute<int> *>(&attribute))
            attributeEditor = new ECAttributeEditor<int>(attribute.GetName(), propertyBrowser_, component, this);
        else if(dynamic_cast<const Foundation::Attribute<bool> *>(&attribute))
            attributeEditor = new ECAttributeEditor<bool>(attribute.GetName(), propertyBrowser_, component, this);*/
        else if(dynamic_cast<const Foundation::Attribute<Color> *>(&attribute))
            attributeEditor = new ECAttributeEditor<Color>(attribute.GetName(), propertyBrowser_, component, this);
        /*else if(dynamic_cast<const Foundation::Attribute<Vector3df> *>(&attribute))
            attributeEditor = new ECAttributeEditor<Vector3df>(attribute.GetName(), propertyBrowser_, component, this);
        else if(dynamic_cast<const Foundation::Attribute<std::string> *>(&attribute))
            attributeEditor = new ECAttributeEditor<std::string>(attribute.GetName(), propertyBrowser_, component, this);*/

        if(attributeEditor)
            QObject::connect(attributeEditor, SIGNAL(AttributeChanged()), this, SIGNAL(AttributesChanged()));
        return attributeEditor;
    }
}