# -*- coding: utf-8 -*-
"""
https://blog.csdn.net/m0_56729804/article/details/134048318?ops_request_misc=&request_id=&biz_id=102&utm_term=python%20opencv%20%E8%AF%BB%E5%8F%96%E8%A7%86%E9%A2%91%E6%B5%81%E6%8A%BD%E5%B8%A7&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-0-134048318.142^v100^pc_search_result_base9&spm=1018.2226.3001.4187
"""
import os
import sys
import cv2
import codecs
from pathlib import Path
import numpy
sys.stdout = codecs.getwriter("utf-8")(sys.stdout.detach())

def find_all_template(im_source, im_search, threshold=0.5, maxcnt=0, rgb=False, bgremove=False):
    # 匹配算法，aircv实际上在代码里写死了用CCOEFF_NORMED，大部分测试的效果，也确实是这个算法更好
    method = cv2.TM_CCOEFF_NORMED
    # 获取匹配矩阵
    res = cv2.matchTemplate(im_source, im_search, method)
    w, h = im_search.shape[1], im_search.shape[0]
    result = []
    while True:
        # 找到匹配最大最小值
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
        top_left = max_loc
        if max_val < threshold:
            break
        # 计算中心点
        middle_point = (top_left[0]+w/2, top_left[1]+h/2)
        # 计算四个角的点，存入结果集
        result.append(dict(
            result=middle_point,
            rectangle=(top_left, (top_left[0], top_left[1] + h), (top_left[0] + w, top_left[1]), (top_left[0] + w, top_left[1] + h)),
            confidence=max_val
        ))
        # 把最匹配区域填充掉，再继续查找下一个
        cv2.floodFill(res, None, max_loc, (-1000,), max_val-threshold+0.1, 1, flags=cv2.FLOODFILL_FIXED_RANGE)
        print(result)
    return result

def Frame_video(video_path, out_path, target_frame=1):
    """
    :param video_path: 需要拆帧的视频路径
    :param out_path: 拆帧后图片保存路径
    :param target_frame: 抽取帧数间隔，默认为1，即1帧保存1张图片
    :return: None
    """
    print("-------------------------视频抽帧-------------------------")
    if not os.path.exists(out_path):
        # 判断文件夹是否存在
        os.makedirs(out_path)
    video = cv2.VideoCapture()  # 初始化一个OpenCV的视频读取对象
    video.open(video_path)
    count = 0  # 记录当前帧数
    image_index = 1000001  # 用于保存图片名称
    frames = int(video.get(cv2.CAP_PROP_FRAME_COUNT))  # 获取帧数
    print('视频共%s帧，抽取%s帧......' % (frames, int(frames/target_frame)))
    while True:
        _, frame = video.read()
        if frame is None:
            print("第%s帧图片无法打开！" % count)
            break
        if count % target_frame == 0:
            if int((image_index-1000000) / int(frames/target_frame) * 100) in [20, 40, 60, 80]:
                print("已提取百分之%s,剩余%s帧......" %
                      (int((image_index-1000000) / int(frames/target_frame) * 100),
                       int(frames / target_frame) - image_index + 1000000))
            save_path = out_path + "%s.png" % image_index
            cv2.imwrite(save_path, frame)
            image_index += 1
        count += 1
    video.release()
    print("视频已全部抽帧完成......")
    print("-------------------------抽帧完成-------------------------")
 
def load_image(image_path):
    """
    :param image_path: 需要加载的图片路径
    :return: 
    """
    image_path = Path(image_path)
    if not image_path.exists():
        print('not exist file')
    try:
        image = cv2.imread(str(image_path))
        return image
    except cv2.error as e:
        print(e)

# "C:/Users/LEIXUEHA/Videos/pic"

def GetFilenameList(file_path):
    """
    :获取存储文件下所有文件名的列表
    :param file_path: 文件路径
    :return: 返回存储文件名的列表
    """
    file_path = Path(file_path)
    if not file_path.exists():
        print('not exist file')
    try:
        # total_num = len(file_path)  #得到文件夹中图像的个数
        # print("文件个数%d" % total_num)
        filenamelist = []
        for filename in os.listdir(file_path):              #listdir的参数是文件夹的路径
            filename = "%s"%(filename)
            print (filename)                               #此时的filename是文件夹中文件的名称
            filenamelist.append(filename)
        return filenamelist
    except cv2.error as e:
        print(e)

# Video_path = r'D:\OneDrive - FORVIA\Pictures\Camera Roll\WIN_20240710_17_22_45_Pro.mp4'
# save_dir = r'C:\Users\LEIXUEHA\Videos\pic/'
def ExtractionVideoFrame(Video_path, save_dir):
    '''
    :视频抽帧
    :param Video_path: 读取的视频路径
    :param save_dir: 存储的图片路径
    '''
    # Video_path = Path(Video_path)
    # save_dir = Path(save_dir)
    # if not Video_path.exists() or save_dir.exists():
    #     print('not exist file')
    # try:
    print("\n-------------------------基础信息-------------------------")
    # Video_path = r'D:\OneDrive - FORVIA\Pictures\Camera Roll\WIN_20240710_17_22_45_Pro.mp4'
    # save_dir = r'C:\Users\LEIXUEHA\Videos\pic/'
    video1 = cv2.VideoCapture()  # 初始化一个OpenCV的视频读取对象
    if not video1.open(Video_path):
        print("无法打开视频，请检查数据！")
        sys.exit()
    fps = video1.get(cv2.CAP_PROP_FPS)  # 获取帧率
    frame_count = int(video1.get(cv2.CAP_PROP_FRAME_COUNT))  # 获取视频的总帧数
    video1.release()  # 清理缓存
    duration = frame_count / fps  # 计算视频的时长（秒）
    print("视频时长为: %ss" % int(duration))
    print("视频帧率为: %sFPS" % int(fps))
    print("视频帧数为: %s" % int(frame_count))
    # Frame = int(input("请输入抽取帧数间隔："))
    Frame_video(Video_path, save_dir)
    # except cv2.error as e:
    #     print(e)

 
if __name__ == '__main__':
    Video_path = r'D:\test\WIN_20240710_09_38_31_Pro.mp4'
    save_dir = r'D:\test\pic/'
    # 返回图标的顶点坐标
    vertex_coordinates_test = [(33, 43), (33, 135), (110, 43), (110, 135)]
    # 读取图片中返回图标的位置
    vertex_coordinates_x = []
    vertex_coordinates_y = []

    # 1)从视频中抽取画面帧
    # ExtractionVideoFrame(Video_path, save_dir)

    # 2)获取所有文件图片名列表
    FilenameList = GetFilenameList(save_dir)

    # 3）遍历加载图片，与图标进行匹配，判断坐标是否在指定范围
    img2 = load_image(r'D:\test\returnButton2.png')
    for i in FilenameList:
        print("图片%s"%i)
        picPath = "D:\\test\pic\%s"%i
        print("加载图片%s"%picPath)
        img1 = load_image(picPath)
        result = find_all_template(img1, img2)
        print(result[0]['rectangle'])#((28, 38), (28, 130), (105, 38), (105, 130))
        for num in range(0,4):
            vertex_coordinates_x.insert(num,result[0]['rectangle'][num][0])
            vertex_coordinates_y.insert(num,result[0]['rectangle'][num][1])
            print("(%d,%d)"%(vertex_coordinates_x[num],vertex_coordinates_y[num]))
        if ((int(vertex_coordinates_x[0]) == int(vertex_coordinates_test[0][0])) and (int(vertex_coordinates_y[0]) == int(vertex_coordinates_test[0][1]))) and\
            ((int((vertex_coordinates_x[1])) == int(vertex_coordinates_test[1][0])) and (int(vertex_coordinates_y[1]) == int(vertex_coordinates_test[1][1]))) and\
            (int((vertex_coordinates_x[2])) == int(vertex_coordinates_test[2][0])) and (int(vertex_coordinates_y[2]) == int(vertex_coordinates_test[2][1])) and\
            (int((vertex_coordinates_x[3])) == int(vertex_coordinates_test[3][0])) and (int(vertex_coordinates_y[3]) == int(vertex_coordinates_test[3][1])):
            print("图片匹配")
        else:
            print("图片%s是异常帧"%i)
            file_handle=open(r'D:\\test\abnormal_frame.txt',mode='w')
            file_handle.write('%s\n' % i)


    # img1 = cv2.imread('C:/Users/LEIXUEHA/Videos/pic/1.png')
    # img2 = cv2.imread('C:/Users/LEIXUEHA/Videos/pic/1_dst_returnButton2.png')
    # result = find_all_template(img1, img2)
    # vertex_coordinates_x = []
    # vertex_coordinates_y = []
    # vertex_coordinates_test = [(28, 38), (28, 130), (105, 38), (105, 130)]
    # for num in range(0,4):
    #     vertex_coordinates_x.insert(num,result[0]['rectangle'][num][0])
    #     vertex_coordinates_y.insert(num,result[0]['rectangle'][num][1])
    #     print("(%d,%d)"%(vertex_coordinates_x[num],vertex_coordinates_y[num]))
    # if ((int(vertex_coordinates_x[0]) == int(vertex_coordinates_test[0][0])) and (int(vertex_coordinates_y[0]) == int(vertex_coordinates_test[0][1]))) and\
    #     ((int((vertex_coordinates_x[1])) == int(vertex_coordinates_test[1][0])) and (int(vertex_coordinates_y[1]) == int(vertex_coordinates_test[1][1]))) and\
    #     (int((vertex_coordinates_x[2])) == int(vertex_coordinates_test[2][0])) and (int(vertex_coordinates_y[2]) == int(vertex_coordinates_test[2][1])) and\
    #     (int((vertex_coordinates_x[3])) == int(vertex_coordinates_test[3][0])) and (int(vertex_coordinates_y[3]) == int(vertex_coordinates_test[3][1])):
    #     print("图片匹配")
    # else:
    #     print("图片异常帧")
    # print(result[0]['rectangle'])#((28, 38), (28, 130), (105, 38), (105, 130))
    # print(result[0]['rectangle'][1][0])#28
    # print(result[0]['rectangle'][1][1])#130



# 文件写入
# file_handle=open('C:/Users/LEIXUEHA/Videos/1.txt',mode='w')

#         for i in filenamelist:
#             file_handle.write('%s' % i)
