# 🚀 Total Evasion - เกมหลบหลีกสุดมันส์บน ESP32

[![Wokwi](https://img.shields.io/badge/Wokwi-Simulate_Now-434D55?style=for-the-badge&logo=wokwi)](https://wokwi.com/projects/439313803343266817)

เกม "Total Evasion" เป็นโปรเจกต์เกมแนวเอาตัวรอด (Survival) ที่สร้างขึ้นสำหรับบอร์ด ESP32 และจอแสดงผล ILI9341 ผู้เล่นจะต้องควบคุมสี่เหลี่ยมสีน้ำเงินเพื่อหลบหลีกฝูงศัตรูที่ตกลงมาอย่างต่อเนื่อง เป้าหมายคือการเอาชีวิตรอดให้นานที่สุดเพื่อทำคะแนนให้สูงที่สุด

โปรเจกต์นี้ถูกพัฒนาและทดสอบทั้งหมดบน **Wokwi Simulator** ซึ่งเป็นเครื่องมือจำลองการทำงานของวงจรอิเล็กทรอนิกส์บนเว็บเบราว์เซอร์ที่ยอดเยี่ยม

## ✨ คุณสมบัติเด่น (Features)

* **การเล่นที่รวดเร็วและท้าทาย:** ความเร็วและจำนวนของศัตรูจะเพิ่มขึ้นตามเวลา
* **ระบบ Dash (พุ่งตัว):** สามารถพุ่งตัวเพื่อหลบหลีกฉุกเฉินได้ด้วยการกดจอยสติ๊กสองครั้ง (Double-tap) และจะเป็นอมตะชั่วขณะ
* **ศัตรูหลากหลายประเภท:** พบกับศัตรู 4 แบบที่มีพฤติกรรมแตกต่างกัน
    * **Normal (สีแดง):** เคลื่อนที่ลงมาตรงๆ
    * **Chaser (สีชมพู):** เคลื่อนที่ไล่ตามผู้เล่นช้าๆ
    * **Bouncer (สีเขียว):** เคลื่อนที่เด้งไปมากับขอบจอ
    * **Dasher (สีขาว):** จะหยุดเล็งเป้าหมายแล้วพุ่งเข้าใส่ผู้เล่นอย่างรวดเร็ว
* **คะแนนตามเวลา:** ยิ่งคุณรอดนานเท่าไหร่ คะแนนก็ยิ่งสูงขึ้นเท่านั้น

## 🛠️ อุปกรณ์ที่ต้องใช้ (Hardware)

* บอร์ด ESP32 DevKit V1
* จอแสดงผล TFT LCD ขนาด 2.4 นิ้ว (ILI9341) 320x240
* โมดูล Analog Joystick
* Push Button (สำหรับเริ่มเกม)
* สายไฟและ Breadboard

## 💻 ซอฟต์แวร์และไลบรารี (Software & Libraries)

* Arduino IDE หรือ PlatformIO
* **ไลบรารีที่จำเป็น:**
    * `Adafruit GFX Library`
    * `Adafruit ILI9341`

## 🎮 วิธีการเล่น (How to Play)

1.  **เป้าหมาย:** เอาชีวิตรอดให้นานที่สุดเพื่อทำคะแนนให้สูงที่สุด
2.  **การควบคุม:**
    * **Analog Joystick:** ใช้ควบคุมทิศทางการเคลื่อนที่ของผู้เล่น
    * **ปุ่ม START (สีเขียว):** กดเพื่อเริ่มเกมจากหน้าจอหลัก หรือเริ่มเกมใหม่หลังจาก Game Over
3.  **ความสามารถพิเศษ (Dash):**
    * กด Joystick ไปในทิศทางใดทิศทางหนึ่ง **สองครั้งเร็วๆ (Double-tap)** เพื่อพุ่งตัวไปในทิศทางนั้น
    * ขณะกำลัง Dash ผู้เล่นจะเป็นอมตะ ไม่โดนโจมตีจากศัตรู

## 🌐 ลองเล่นบน Wokwi Simulator!

คุณสามารถทดลองเล่นเกมนี้ได้ทันทีบนเบราว์เซอร์ของคุณผ่าน Wokwi Simulator โดยไม่ต้องต่อวงจรจริง!

**[คลิกที่นี่เพื่อเปิดโปรเจกต์บน Wokwi](https://wokwi.com/projects/439313803343266817)**

## 📸 ภาพหน้าจอ (Screenshot)

![image](https://private-user-images.githubusercontent.com/181745491/481452049-d8ac4718-1ff0-49b5-9cc0-229f46fceeba.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NTYwOTM0MjcsIm5iZiI6MTc1NjA5MzEyNywicGF0aCI6Ii8xODE3NDU0OTEvNDgxNDUyMDQ5LWQ4YWM0NzE4LTFmZjAtNDliNS05Y2MwLTIyOWY0NmZjZWViYS5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjUwODI1JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI1MDgyNVQwMzM4NDdaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT0wMDJiZjMwZmFiYjBjOTNjZmQ0YTE3MDYwNWJkYzk5MTNiYzQ4NDI3Y2FiNjNiMTAwZTY3ZjcxNDhmMjQ0ZWNkJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCJ9.frjbFYrMGOiUc8OlIgWpawnqfoumVqSVkAf2mI5yoOY)
