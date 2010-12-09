// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_OgreRenderingModule_OgreMeshAsset_h
#define incl_OgreRenderingModule_OgreMeshAsset_h

#include <boost/shared_ptr.hpp>
#include "IAsset.h"
#include <OgreMesh.h>

class OgreMeshAsset : public IAsset
{
    Q_OBJECT;
public:
    OgreMeshAsset(AssetAPI *owner, const QString &type_, const QString &name_)
    :IAsset(owner, type_, name_)
    {
    }

    ~OgreMeshAsset();

    virtual bool DeserializeFromData(const u8 *data_, size_t numBytes);

    virtual void Unload();

    /// Returns an empty list - meshes do not refer to other assets.
    virtual std::vector<AssetReference> FindReferences() const { return std::vector<AssetReference>(); }

    void SetDefaultMaterial();

    /// This points to the loaded mesh asset, if it is present.
    Ogre::MeshPtr ogreMesh;

    /// Specifies the unique mesh name Ogre uses in its asset pool for this mesh.
//    QString ogreAssetName;

//    std::vector<QString> originalMaterials;
};

typedef boost::shared_ptr<OgreMeshAsset> OgreMeshAssetPtr;

#endif
