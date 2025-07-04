/*
  ******************************************************
  ⚠️ تحذير هام – جهاز تحفيز العضلات ⚠️
  ******************************************************
  
  هذا الكود يتحكم في وحدة تحفيز عضلي.
  
  ❗ الاستخدام المطوّل للجهاز على عضلات الجسم قد يسبب إجهادًا عضليًا أو تهيجًا جلديًا.
  ❗ لا يُوصى باستخدام ترددات أعلى من 50 هرتز أو زيادة الطاقة الكهربائية دون استشارة مختص.
  ❗ يُمنع الاستخدام على مناطق حساسة من الجسم (مثل الرأس أو القلب).
  ❗ لا تستخدم الجهاز إن كنت تعاني من أمراض في القلب أو لديك جهاز منظم ضربات.
  ❗ تأكد دائمًا من إيقاف التشغيل قبل توصيل أو فصل الجهاز من العضلات.

  📌 هذا الكود والجهاز لأغراض تعليمية وتجريبية فقط، وليس مخصصًا للاستخدام الطبي أو العلاجي دون إشراف طبي.


  ******************************************************
*/


/*
  📋 حدود التشغيل الآمن لوحدة تحفيز العضلات:

  ▸ التردد (Frequency):
    - المدى: من 0 إلى 50 هرتز (Hz)
    - ⚠️ يوصى بعدم تجاوز 50 هرتز لتجنب الإجهاد العصبي أو العضلي.

  ▸ قوة النبضة (Duty Cycle):
    - النطاق: من 0% إلى 100%
    - ⚠️ يُفضل عدم تجاوز 70% لفترات طويلة لتفادي التحفيز الزائد.

  ▸ توقيت التشغيل (ON Time):
    - المدى: من 100 إلى 2000 مللي ثانية
    - ⏱️ أقصى مدة تشغيل مستمرة: 2 ثانية

  ▸ توقيت الإطفاء (OFF Time):
    - المدى: من 0 إلى 5000 مللي ثانية )
    - ⏱️ أقصى مدة إيقاف: 5 ثوانٍ

  ❗ هذه القيم تم تحديدها لضمان السلامة أثناء الاستخدام.
*/


/*
 ربط الجهاز مع المتحكم 
 Vcc - 5V
 GND - GND 
 PWM - PIN 3

 ربط الشاشة مع متجكم 
 Vcc - 5V
 GND - GND 
 SDA - A4
 SCL - A5
 
 */

#include <Wire.h>                      // مكتبة للتعامل مع بروتوكول I2C (ضرورية لعمل شاشة LCD I2C)
#include <LiquidCrystal_I2C.h>        // مكتبة للتحكم في شاشة LCD عبر I2C
#include <HTP_EMS0002.h>               // مكتبة خاصة بجهاز التحفيز العضلي الكهربائي

#define STIM_PIN 3                    // تعريف رقم منفذ الإخراج المستخدم للتحفيز الكهربائي
#define FREQ_BUTTON_PIN 4             // تعريف رقم المنفذ الموصل بزر تعديل التردد
#define DUTY_BUTTON_PIN 5             // تعريف رقم المنفذ الموصل بزر تعديل الشدة (duty cycle)

LiquidCrystal_I2C lcd(0x27, 16, 2);   // إنشاء كائن للشاشة LCD باستخدام العنوان 0x27 وعدد الأحرف 16x2
HTP_EMS0002 stim(STIM_PIN);           // إنشاء كائن لجهاز التحفيز وتحديد منفذ التحفيز

int frequency = 10;                   // التردد الابتدائي للتحفيز الكهربائي (بالهرتز)
int dutyCycle = 10;                  // شدة النبض الابتدائية (نسبة مئوية)

unsigned long lastDebounceTime = 0;   // متغير لتخزين آخر وقت تم فيه الضغط على الزر
const unsigned long debounceDelay = 200; // مدة مقاومة الاهتزاز للزر لمنع التكرار العشوائي (200 مللي ثانية)

void setup() {
    pinMode(FREQ_BUTTON_PIN, INPUT_PULLUP);  // تعيين زر التردد كإدخال مع مقاومة سحب داخلية
    pinMode(DUTY_BUTTON_PIN, INPUT_PULLUP);  // تعيين زر الشدة كإدخال مع مقاومة سحب داخلية

    lcd.begin();                  // بدء تشغيل الشاشة LCD
    lcd.backlight();              // تشغيل الإضاءة الخلفية للشاشة

    stim.begin();                // بدء تهيئة جهاز التحفيز العضلي
    stim.setFrequency(frequency); // تعيين التردد الابتدائي للتحفيز
    stim.setDutyCycle(dutyCycle); // تعيين شدة النبض (نسبة مئوية)
    stim.setOnOffTimers(1500, 1000); // تعيين مدة التشغيل 1500 مللي ثانية، والإيقاف 1000 مللي ثانية
    stim.start();                 // بدء تشغيل التحفيز

    lcd.setCursor(0, 0);          // تحريك المؤشر إلى أول سطر في الشاشة
    lcd.print("Freq: ");          // طباعة "Freq: " على الشاشة
    lcd.print(frequency);         // طباعة قيمة التردد الحالي
    lcd.print(" Hz");             // طباعة "Hz" بجانب التردد

    lcd.setCursor(0, 1);          // الانتقال إلى السطر الثاني في الشاشة
    lcd.print("Duty: ");          // طباعة "Duty: " على الشاشة
    lcd.print(dutyCycle);         // طباعة قيمة الشدة الحالية
    lcd.print(" %");              // طباعة علامة النسبة المئوية
}

void loop() {
    stim.update();  // تحديث حالة جهاز التحفيز (يجب استدعاؤها باستمرار لضمان الاستمرارية)

    // التحقق من حالة زر التردد وتحديث القيمة عند الضغط
    if (digitalRead(FREQ_BUTTON_PIN) == LOW && (millis() - lastDebounceTime > debounceDelay)) {
        frequency += 10;              // زيادة التردد بمقدار 10 هرتز
        if (frequency > 50) frequency = 10;  // إعادة التردد إلى 10 إذا تجاوز 50
        stim.setFrequency(frequency);       // تحديث التردد في جهاز التحفيز
        updateLCD();                        // تحديث المعلومات المعروضة على الشاشة
        lastDebounceTime = millis();        // تحديث وقت آخر ضغط للزر
    }

    // التحقق من حالة زر الشدة وتحديث القيمة عند الضغط
    if (digitalRead(DUTY_BUTTON_PIN) == LOW && (millis() - lastDebounceTime > debounceDelay)) {
        dutyCycle += 10;              // زيادة الشدة بمقدار 10%
        if (dutyCycle > 100) dutyCycle = 10; // إعادة الشدة إلى 10% إذا تجاوزت 100%
        stim.setDutyCycle(dutyCycle);       // تحديث الشدة في جهاز التحفيز
        updateLCD();                        // تحديث المعلومات المعروضة على الشاشة
        lastDebounceTime = millis();        // تحديث وقت آخر ضغط للزر
    }
}

void updateLCD() {
    lcd.clear();                   // مسح محتويات الشاشة بالكامل
    lcd.setCursor(0, 0);           // تحريك المؤشر إلى أول سطر
    lcd.print("Freq: ");           // طباعة "Freq: "
    lcd.print(frequency);          // طباعة قيمة التردد
    lcd.print(" Hz");              // طباعة "Hz"

    lcd.setCursor(0, 1);           // تحريك المؤشر إلى السطر الثاني
    lcd.print("Duty: ");           // طباعة "Duty: "
    lcd.print(dutyCycle);          // طباعة قيمة الشدة
    lcd.print(" %");               // طباعة "%"
}
