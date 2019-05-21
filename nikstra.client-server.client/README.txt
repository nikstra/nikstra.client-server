
Issues I've run into when developing this
=========================================

* I got a linker error on first build:
LNK2019	unresolved external symbol _main referenced in function "int __cdecl invoke_main(void)" (?invoke_main@@YAHXZ)
Resolved by changeing project configuration. Linker->System->SubSystem should be Windows.
