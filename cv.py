import cv2                      # подключение модуля да работы с изображением
import numpy as np
import compound                    # подключение модуля да работы с COM портом
import time




cap = cv2.VideoCapture(0)     # подключение к камере


port = compound.Connect(0,3)   #подключение к ардуино

flag, img = cap.read()
time.sleep(6)               # фокусировка камеры
port.write(b'R')            # сигнал для начала детектирования


edge = 220         


while True:
    
    flag, img = cap.read()

    height, width = img.shape[:2]
    
    img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)  
    low_blue = np.array((100,70,90),np.uint8)
    high_blue = np.array((150,255,255),np.uint8)     #создание маски синего цвета
    try:

        img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)
        mask_blue = cv2.inRange(img_hsv, low_blue, high_blue)

        moments = cv2.moments(mask_blue,1)

        dM01 = moments['m01']
        dM10 = moments['m10']
        dArea = moments['m00']

        x=0
        
        if dArea > 1000 :
            x = int(dM10 / dArea)
            y = int(dM01 / dArea)

        #нахождение положения объекта на кадре и отправка сигналов на ардуино

        if (x>(width/2+edge)) and x!=0:
            port.write(b'R')
            
        if (x<(width/2-edge)) and x!=0:
            port.write(b'L')
            
        if (x<(width/2+edge)) and (x>(width/2-edge)) and x!=0:
            port.write(b'F')
            if (edge>140):
                edge = edge - 1
        
        
        # получение сигнала о том что цель была захвачена
        if (port.inWaiting()>0):
            break
            
            
        
        
                
    except:
        cap.release()
        raise

cap.release()