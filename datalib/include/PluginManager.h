#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

interface class IPluginBase;
interface class IGridPlugin;
interface class IOrdersPlugin;

public ref class PluginManager
{
public:
    PluginManager(String^);

    void LoadPlugins();
    void UpdatePlugins();

    static property List<IPluginBase^>^ AllPlugins
    {
        List<IPluginBase^>^ get() { return m_AllPlugins; }
    }
    static property List<IGridPlugin^>^ GridPlugins
    {
        List<IGridPlugin^>^ get() { return m_GridPlugins; }
    }
    static property List<IOrdersPlugin^>^ OrderPlugins
    {
        List<IOrdersPlugin^>^ get() { return m_OrdersPlugins; }
    }

private:

    String^                        m_Path;

    static List<IPluginBase^>^     m_AllPlugins;
    static List<IGridPlugin^>^     m_GridPlugins;
    static List<IOrdersPlugin^>^   m_OrdersPlugins;
};

} // end namespace FHUI
