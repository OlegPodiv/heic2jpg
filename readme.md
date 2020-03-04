# heic2jpg

Convert pictures in HEIC format to JPG format（HEIC格式的图片转换成JPG格式）

（1）关于HEIC格式
   在升级 iOS 11 之后，iPhone 7 及更新的设备内的照片存储将不再用 JPEG 了，而采用了一种新的图片格式 HEIF（发音同 heef），在 iOS 中对应的文件后缀为 .heic。

 所以heic是HEIF的一种图片封装格式，HEIF格式的全名为 High Efficiency Image File Format（高效率图档格式）,其相关参考资料如下：
  
   HEIF格式解析：https://www.jianshu.com/p/b016d10a087d 

   HEIF 和 HEVC 研究：https://blog.csdn.net/Philm_iOS/article/details/81200989
   
（2）windows下heic支持
 
   正常情况下，heic图片文件是不能在windows中打开的。

   想要在windows中查看heic格式的照片，可以将heic图片转换成jpg图片。因此本项目参考heif解析扩展库的基础上通过将heic转成jpg格式图片实现预览

首先介绍下本项目依赖相关工程：

   下载HEIF/HEIC扩展库：https://github.com/nokiatech/heif
   
   ffmpeg编解码库：http://ffmpeg.org/download.html#build-windows
   
（3）本工程提供什么内容
   
   本工程公共编译heif解析库 + ffmpeg编解码库(图片编码转换)提供导出动态库heic2jpeg.dll，和导出接口：
   
   ITL_ERT EuHeic2jpeg(const wchar_t *wzSrcFilePath, const wchar_t *wzDstJpgPath);
   
   ITL_ERT EuHeic2Thumbnail(const wchar_t *wzSrcFilePath, const wchar_t *wzDstThumbnailPath);
   
 
（4）本工程已经包含完整的heif库 + ffmpeg依赖库，下载代码即可使用vs2017编译测试
    祝你好运!
