# 镜头检测实验报告

3150104785 郝广博 计算机科学与技术1502

## Project Introduction

编写程序进行视频镜头检测（2种以上算法）

输入：视频片段

输出：发生镜头切换处的帧编号

## Technical Details

### 视频、镜头、帧

动态的视频可以理解为是由一幅幅静态的图像构成的，这每一幅图像都称作是帧。

而视频往往是由一个个片段构成的，这些片段称为是镜头，同一组镜头中，相邻图像帧的特征保持稳定。如果相邻图像帧之间的特征发生了明显的变化，则可以认为是镜头切换了。利用这一基本的原理，可以进行镜头检测和分割的算法设计。

通过基本原理，可以发现镜头检测本质上就是要对相邻帧之间的图像相似程度进行判断，如果计算得到的图像相似度很低，就可以认为是镜头发生了切换。

## Experiment Results



## References