KA3D Readme File

Overview
--------

KA3D is a scene graph based 3D-engine with a tool-chain for 3dsmax (7 and newer).
KA3D is the engine used in 'Making Game Demo' book by Chad Walker, Eric Walker
and me (Jani Kajala), so it has pretty extensive documentation. :P
Lot of documentation is also included in this package, see ka3d\docs directory.

One good side of KA3D is that the code base is very portable, currently running on:
* Win32/DirectX 9
* Windows Mobile/OpenGL ES 1.x
* Symbian Series 60 1st, 2nd and 3rd Ed. using software rasterizer
* Nokia NGI/N-Gage using Nokia's proprietary N3D rasterizer
* Qualcomm BREW using software rasterizer
* Playstation Portable
* Nintendo DS (work-in-progress)

KA3D has also been used in several real-life projects:
* Bounce - Bon Voyage (NGI/N-Gage title developed by Rovio, published by Nokia)
* Puzzlescape (PSP title developed by Farmind, published by O3)
* Zaxdemo (3rd person demo shipped with 'Making Game Demo' book)
* Burnout BREW 3D version (title developed by Rovio, published by Electronic Arts)
* Various university student projects, for example, in SMU/Guildhall


Install
-------

(Note: if you installed KA3D using installer, everything should be ok already)

If you installed KA3D using the installer, it added
"c:\ka3d\SystemDLL" to your user PATH variable and
set new environment variable KA3D=c:\ka3d

If it didn't (or if you installed from zip/manually) then
you need to setup those manually. 
(My Computer -> Properties -> Advanced -> Environment Variables)


How to get started
------------------

1) Test that Zaxdemo runs ok (in "samples\zaxdemo")

2) Check source code tutorials from "docs\KA3D Graphics Programming Guide.pdf" 
   and associated source code in "samples\tutorials"
   
3) Install 3dsmax Exporter plugin from "tools\bin\hgrexport-max7.dle"
   by copying it to "C:\Program Files\Autodesk\3dsMax8\plugins"


Requirements
------------

* PC with Windows XP, DirectX 9 or newer installed etc

* Visual Studio 2005 and DirectX 9 SDK (if you want to compile examples)


License
-------

This release is licensed under GPL 3.0, see docs\license.txt.


Changes by version
------------------

1.4:
- new setup project
- documentation for the graphics programming tutorials included in the package
- added platform support: BREW / Custom Software Renderer 
  (not publicly released; contact jani@rovio.com if you're interested in that)
- news: KA3D is used in 'Burnout' BREW 3D version released Q3-07 by Electronic Arts Mobile

1.3:
- zaxdemo: v0.8 improvements, animated dynamic objects, scripting, etc.

1.2:
- portability improvements
- added platform support: Nokia NGI/N-Gage (not publicly released; contact jani@rovio.com)
- added 2ntx texture conversion tool (not needed on Win32/DirectX build)
- news: KA3D is used in 'Bounce - Bon Voyage' Nokia's N-Gage 1st party 3D title developed by Rovio

1.1:
- added platform support: OpenGL ES 1.x (not publicly released; contact jani@rovio.com)
- added OS support: Windows Mobile (not publicly released; contact jani@rovio.com)
- added OS support: Symbian Series 60 1st, 2nd and 3rd Ed. 
  (not publicly released; contact jani@rovio.com if you're interested in that)
- news: KA3D is used in Farmind's Playstation Portable title 'Puzzlescape' published by O3

1.0:
- original release with 'Making Game Demo' book


Contact information
-------------------

The 3D-engine has been designed & programmed by Jani Kajala.
For any questions please contact me by email: jani.kajala at rovio.com

All Zaxdemo 3D-content has been done by Chad and Eric Walker.
Chad also has done extensive testing during the whole life-cycle of
the engine so he knows the engine inside out as well. :)
You can contact Chad by email as well: walkerbg at msn.com
