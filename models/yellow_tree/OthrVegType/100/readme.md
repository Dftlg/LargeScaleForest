Common::renderingVegMesh 渲染模型体素颜色，匹配了两个项目功能

第一个功能文件夹为EachPartSkelCubic_Index

可通过SpatialVoxelExtraction项目输入模型体素，并进行模型体素的骨架网格体素提取操作。并将骨架模型的体素索引存入到EachPartSkelCubic_Index文件夹中，在进行VegaInterpolationRendering的renderingVegMesh 渲染

第二个功能文件夹为ObjMappingVoxel

可通过项目VegFileDivideMaterialsRegions输入模型的Obj进行模型体素化，并提取出模型每个Group组对应的体素存入到ObjMappingVoxel文件夹中，在进行VegaInterpolationRendering，请注意修改tree100Rendering.veg文件



ObjeMappingVoxel.cpp

RemoveOverlappingVoxelsInTwoGroups function

![image-20220104162140318](D:\GraduationProject\New-LargeScaleForest\LargeScaleForest\models\yellow_tree\OtherVegType\100\readmeImage/1.png)

一棵树木可以分为4个部分(针对不同树木不同)

对于每个部分的体素

可用VegaInterpolationRendering渲染出来

![image-20220104162140318](D:\GraduationProject\New-LargeScaleForest\LargeScaleForest\models\yellow_tree\OtherVegType\100\readmeImage/2.png)

此时对于不同部分有大量的重叠，尤其是细枝与叶片

![image-20220104162140318](D:\GraduationProject\New-LargeScaleForest\LargeScaleForest\models\yellow_tree\OtherVegType\100\readmeImage/6.png)

![image-20220104162140318](D:\GraduationProject\New-LargeScaleForest\LargeScaleForest\models\yellow_tree\OtherVegType\100\readmeImage/4.png)

为此我们将细枝与叶片相交的部分都算作叶片

同理将树木从高到低

细枝与叶片相交的部分都算作叶片

粗枝与细枝相交部分算作细枝

树干与粗枝相交部分算作粗枝