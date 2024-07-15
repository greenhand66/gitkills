# -*- coding:utf-8 -*-
import tkinter as tk
# from tkinter import filedialog

# 创建主窗口
root_window = tk.Tk()

# 设置主窗口的标题
root_window.title('视频抽帧，坏帧识别') 

# 定义主窗口的显示尺寸
root_window.geometry('900x700')

# 设置窗体背景色
root_window["background"] = "#C9C9C9"

# 开启主循环，让窗口处于显示状态
root_window.mainloop()

#隐藏主窗口
# root_window.withdraw()
root_window["background"] = "#C9C9C9"
# root_window.deiconify()


# option = tk.IntVar()

# label = tk.Label(root, text="Select an option:")
# label.pack()


# root.deiconify()  # 显示窗口
# root.mainloop()

# 选择一个视频文件并获取其路径
# file_path = filedialog.askopenfilename(title='请选择一个视频文件', filetypes=[('Video','.mkv .mp4'),('All Files', '*')],initialdir='D:\\')
# print(file_path)
