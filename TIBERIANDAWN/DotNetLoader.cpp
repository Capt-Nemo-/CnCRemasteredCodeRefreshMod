// #include "DotNetLoader.h"
//
// using namespace System;
//
// Reflection::Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args)
// {
// 	array<Byte>^ ba;	
// 	auto stm = Reflection::Assembly::GetExecutingAssembly()->GetManifestResourceStream("CncDotNet.dll");
//
// 	try
// 	{
// 		ba = gcnew array<Byte>(stm->Length);
// 		stm->Read(ba, 0, stm->Length);
// 	}
// 	finally
// 	{
// 		stm->Close();
// 	}
//
// 	return Reflection::Assembly::Load(ba);
// }
//
// void DotNetLoader::Init()
// {
// 	AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(&CurrentDomain_AssemblyResolve);	
// }
