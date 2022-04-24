echo "running 00..............."
cd 00_vulkanBaseCode; make test; cd ..

echo "running 01..............."
cd ./01_vulkanWindow; make test; cd ..

echo "running 02..............."
cd ./02_vulkanInstance; make test; cd ..

echo "running 03..............."
cd ./03_vulkanValidation; make test; cd ..

echo "running 04..............."
cd ./04_vulkanPhysicalDevice; make test; cd ..

echo "running 05..............."
cd ./05_vulkanQueueFamilies; make test; cd ..

echo "running 06..............."
cd ./06_vulkanLogicalDevice; make test; cd ..

echo "running 07..............."
cd ./07_vulkanWindowSurface; make test; cd ..

echo "running 08..............."
cd ./08_vulkanSwapchain; make test; cd ..

echo "running 09..............."
cd ./09_vulkanImageView; make test; cd ..

echo "running 10..............."
cd ./10_vulkanPipeline; make test; cd ..

echo "running 11..............."
cd ./11_vulkanRenderPass; make test; cd ..

echo "running 12..............."
cd ./12_vulkanDraw; make test; cd ..

