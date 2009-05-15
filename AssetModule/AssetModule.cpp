// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "AssetManager.h"
#include "AssetModule.h"
#include "OpenSimProtocolModule.h"
#include "RexProtocolMsgIDs.h"
#include "RexUUID.h"
#include "UDPAssetProvider.h"

using namespace OpenSimProtocol;
using namespace RexTypes;

namespace Asset
{
    AssetModule::AssetModule() : ModuleInterfaceImpl(type_static_),
        inboundcategory_id_(0)
    {
    }

    AssetModule::~AssetModule()
    {
    }

    // virtual
    void AssetModule::Load()
    {
        LogInfo("Module " + Name() + " loaded.");
        
        AutoRegisterConsoleCommand(Console::CreateCommand(
            "RequestAsset", "Request asset from server. Usage: RequestAsset(uuid,assettype)", 
            Console::Bind(this, &AssetModule::ConsoleRequestAsset)));
    }

    // virtual
    void AssetModule::Unload()
    {
        LogInfo("Module " + Name() + " unloaded.");
    }

    // virtual
    void AssetModule::Initialize()
    {
        manager_ = AssetManagerPtr(new AssetManager(framework_));
        framework_->GetServiceManager()->RegisterService(Foundation::Service::ST_Asset, manager_);
        
        udp_asset_provider_ = Foundation::AssetProviderPtr(new UDPAssetProvider(framework_));
        manager_->RegisterAssetProvider(udp_asset_provider_);
        
        LogInfo("Module " + Name() + " initialized.");
    }
    
    // virtual
    void AssetModule::PostInitialize()
    {
        inboundcategory_id_ = framework_->GetEventManager()->QueryEventCategory("OpenSimNetworkIn");
        
        if (inboundcategory_id_ == 0 )
            LogWarning("Unable to find event category for OpenSimNetwork events!");
    }

    // virtual
    void AssetModule::Update(Core::f64 frametime)
    {
        if (manager_)
            manager_->Update(frametime);
    }

    // virtual 
    void AssetModule::Uninitialize()
    {
        manager_->UnregisterAssetProvider(udp_asset_provider_);
    
        framework_->GetServiceManager()->UnregisterService(manager_);
        manager_.reset();
        
        LogInfo("Module " + Name() + " uninitialized.");
    }
    
    Console::CommandResult AssetModule::ConsoleRequestAsset(const Core::StringVector &params)
    {
        if (params.size() != 2)
        {
            return Console::ResultFailure("Usage: RequestAsset(uuid,assettype)");
        }

        manager_->RequestAsset(params[0], params[1]);
        return Console::ResultSuccess();
    }
    
    bool AssetModule::HandleEvent(
        Core::event_category_id_t category_id,
        Core::event_id_t event_id, 
        Foundation::EventDataInterface* data)
    {
        if ((category_id == inboundcategory_id_))
        {
            if (udp_asset_provider_)
                return checked_static_cast<UDPAssetProvider*>(udp_asset_provider_.get())->HandleNetworkEvent(data);
        }     
       
        return false;
    }    
}

extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Framework *framework);
void SetProfiler(Foundation::Framework *framework)
{
    Foundation::ProfilerSection::SetProfiler(&framework->GetProfiler());
}

using namespace Asset;

POCO_BEGIN_MANIFEST(Foundation::ModuleInterface)
    POCO_EXPORT_CLASS(AssetModule)
POCO_END_MANIFEST

