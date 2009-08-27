#include "StdAfx.h"
#include "PluginManager.h"

using namespace System::IO;
using namespace System::Reflection;

namespace FHUI
{

PluginManager::PluginManager(GameData^ gd, String^ path)
    : m_GameData(gd)
    , m_Path(path)
{
    if ( m_AllPlugins == nullptr )    m_AllPlugins = gcnew List<IPluginBase^>;
    if ( m_GridPlugins == nullptr )   m_GridPlugins = gcnew List<IGridPlugin^>;
    if ( m_OrdersPlugins == nullptr ) m_OrdersPlugins = gcnew List<IOrdersPlugin^>;    
}

////////////////////////////////////////////////////////////////
// Plugins

void PluginManager::LoadPlugins()
{
    DirectoryInfo ^dir = gcnew DirectoryInfo(m_Path);

    for each( FileInfo ^f in dir->GetFiles("fhui.*.dll"))
    {
        if( f->Name->ToLower() == "fhui.datalib.dll" )
            continue;

        try
        {
            Assembly ^assembly = Assembly::LoadFrom(f->FullName);

            // Walk through each type in the assembly
            for each( Type ^type in assembly->GetTypes() )
            {
                if( type->IsClass && type->IsPublic )
                {
                    if( type->GetInterface("FHUI.IPluginBase") )
                    {
                        IPluginBase^ plugin = safe_cast<IPluginBase^>(Activator::CreateInstance(type));
                        m_AllPlugins->Add(plugin);
                        
                        IGridPlugin^ gridPlugin = dynamic_cast<IGridPlugin^>(plugin);
                        if( gridPlugin )
                            m_GridPlugins->Add(gridPlugin);

                        IOrdersPlugin^ ordersPlugin = dynamic_cast<IOrdersPlugin^>(plugin);
                        if( ordersPlugin )
                            m_OrdersPlugins->Add(ordersPlugin);
                    }
                }
            }
        }
        catch( Exception ^ex )
        {
            throw gcnew FHUIPluginException(
                String::Format("Error occured while loading plugin: {0}\r\n Error message: {1}",
                    f->Name, ex->Message),
                ex );
        }
    }
}

void PluginManager::UpdatePlugins()
{
    for each( IGridPlugin ^plugin in m_GridPlugins )
    {
        plugin->SetGameData(m_GameData);
    }
}

} // end namespace FHUI

