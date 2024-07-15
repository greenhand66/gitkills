# -*- coding:utf-8 -*-
import tkinter as tk
from tkinter import filedialog

video_path = {'file_path':""}

def FileChooseWindow():
    # 选择一个视频文件并获取其路径
    file_path = filedialog.askopenfilename(title='请选择一个视频文件', filetypes=[('Video','.mkv .mp4'),('All Files', '*')],initialdir='D:\\')
    print(file_path)
    video_path['file_path'] = file_path

if __name__ == '__main__':
    # 创建主窗口
    root_window = tk.Tk()

    # 设置主窗口的标题
    root_window.title('视频抽帧，坏帧识别') 

    # 定义主窗口的显示尺寸
    root_window.geometry('900x700')

    # 设置窗体背景色
    root_window["background"] = "#C9C9C9"

    # 添加按钮，以及按钮的文本，并通过command参数设置关闭窗口的功能
    button=tk.Button(root_window, text="选择抽帧的视频文件", command=FileChooseWindow)
    button.pack()

    # 添加文本内容，设置字体的前景色和背景色，和字体类型、大小
    text=tk.Label(root_window, text=video_path['file_path'], bg="yellow", fg="red", font=("Times", 20, "bold italic"))
    # 将文本内容放置在主窗口内
    text.pack()

    # 添加按钮，以及按钮的文本，并通过command参数设置关闭窗口的功能
    button=tk.Button(root_window, text="退出", command=root_window.quit)
    button.pack(side="bottom")

    # 开启主循环，让窗口处于显示状态
    root_window.mainloop()


#隐藏主窗口
# root_window.withdraw()
# root_window["background"] = "#C9C9C9"
# root_window.deiconify()


# option = tk.IntVar()

# label = tk.Label(root, text="Select an option:")
# label.pack()


# root.deiconify()  # 显示窗口
# root.mainloop()

