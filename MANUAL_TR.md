# Better RGB by TheYamo — Kullanım Kılavuzu (Türkçe)

Tongfang/ITE8291 tabanlı klavyeler (VID_048D, PID_600B) için özel per-key RGB kontrol uygulaması.

---

## 1. Başlamadan önce — önemli kurulum adımı

Bu uygulama, klavyenin USB arayüzüyle doğrudan konuşuyor. Üretici firmanın kendi RGB servisi ("GCUBridge") da aynı arayüzle konuşuyor — ikisi aynı anda ışıkları kontrol etmeye çalışırsa titreme, çakışma, hatta nadir durumlarda klavyenin gömülü kontrolcüsünün (EC) tamamen kilitlenmesi gibi sorunlar yaşanabilir.

**GCUBridge Windows servisini kapatmana gerek yok.** Kapatırsan, Kontrol Merkezi yazılımının RGB dışındaki diğer özelliklerini (fan eğrileri, performans modları, vs. — sisteminde yönettiği her ne varsa) kaybedersin.

**Bunun yerine yapman gereken:**

1. Üreticinin **Kontrol Merkezi** uygulamasını aç.
2. Klavye aydınlatması / RGB bölümünü bul.
3. O uygulama içinden **klavye aydınlatmasını KAPAT**.

Aydınlatma orada kapalı olduğu sürece, GCUBridge servisi diğer işlevleri için arka planda normal şekilde çalışmaya devam edebilir ve RGB arayüzüne hiç dokunmaz — böylece bu uygulama onu güvenle kontrol edebilir.

⚠️ **Kontrol Merkezi'nde aydınlatma açıkken bu uygulamayı asla aynı anda çalıştırma.** Her zaman önce orada kapat.

---

## 2. Kaynak koddan derleme

Eğer sana sadece kaynak dosyalar verildiyse, kendin derlemen gerekecek. İhtiyacın olanlar:
- `mingw-w64` (Windows için gcc), `windres` dahil
- [hidapi](https://github.com/libusb/hidapi) kaynak kodu (sadece `hidapi/hidapi.h` ve `windows/hid.c` gerekli)

Aynı klasörde bulunması gereken dosyalar:
- `rgb_gui.c` (ana kaynak kod)
- `resource.rc`
- `resource.h`
- `app.manifest`
- `app.ico`

Derleme (o klasörden, iki adım):
```
windres resource.rc -O coff -o resource.o
gcc rgb_gui.c resource.o HIDAPI_YOLU\windows\hid.c -IHIDAPI_YOLU\hidapi -o rgb_gui.exe -lsetupapi -lhid -lm -mwindows -lcomdlg32 -lcomctl32 -lshell32
```
`HIDAPI_YOLU` kısmını hidapi kaynak klasörünü nereye koyduysan onunla değiştir.

**Not:** yol içinde boşluk içeren bir klasör varsa, o yolu tamamen çift tırnak içine al.

---

## 3. Uygulamayı çalıştırma

`rgb_gui.exe`'ye çift tıkla (ya da terminalden çalıştır). Herhangi bir konsol penceresi açılmaz — bu normal, pencereli bir uygulama.

Klavye bulunamazsa ("Could not open device" hatası çıkarsa), en yaygın sebep Kontrol Merkezi'nde aydınlatmanın hâlâ açık olmasıdır — 1. Bölüm'e geri dön.

---

## 4. Arayüz

- **Select Mode** — üstteki açılır kutu. Bir öğe seçmek o efekti anında başlatır ve altında sadece o efekte ait ayarları gösterir.
- **Stop (Turn Off Lights)** — mevcut efekti durdurur ve klavyeyi kapatır.
- **Brightness** — alttaki kaydırıcı, hangi mod aktif olursa olsun her zaman görünür. Genel çıktı parlaklığını (0-100%) ölçekler.
- **Active Mode** yazısı — şu an hangi efektin çalıştığını gösterir.

Pencereyi küçültmek, onu görev çubuğu yerine sistem tepsisine (saat yanındaki gizli simgeler alanı) gönderir — uygulama arka planda çalışmaya devam eder. Tray simgesine çift tıklayarak pencereyi geri getirebilir, ya da sağ tıklayarak Aç/Çıkış menüsüne ulaşabilirsin.

---

## 5. Modlar ve ayarları

### Breathing (Nefes Alma)
Tüm klavye tek bir renkte yanıp sönerek nefes alır.
- **Color** — "Pick Color" ile renk seç.
- **Speed** — nefes alma döngüsünün ne kadar hızlı olacağı.
- **Smoothness** — parlaklık geçişinin ne kadar kademesiz göründüğü. Yüksek değer = göz farkı algılamayacağın kadar pürüzsüz.

### Wave (Dalga)
Klavye üzerinde hareket eden bir renk geçişi.
- **Angle** — 0-359°. 0° soldan sağa, 90° yukarıdan aşağıya akar, diğer değerler çapraz akar. Sürüklerken tam dereceyi gösteren canlı bir balon çıkar.
- **Speed** — geçişin ne kadar hızlı hareket edeceği.

### Sparkle (Rastgele Parıltı)
Rastgele tuşlar kısa süreliğine parlar.
- **Speed** — her parıltının ne kadar sürede söneceği (yüksek hız = daha kısa ömürlü, daha çabuk parıltılar).
- **Density** — aynı anda kaç tuşun yanık olacağı (3-20), speed'den bağımsız.

### Reactive (Tuşa Basınca Tepki)
Bastığın tuşlar yanar, bıraktığında söner.
- **Color** — sabit bir renk seç, ya da "Random" ile her basışta farklı rastgele renk.
- **Duration** — Short / Medium / Long: tuşu bıraktıktan sonra ışığın ne kadar sürede (sönerek) kapanacağı. Not: tuş basılı tutulduğu sürece bu ayardan bağımsız olarak tam parlaklıkta kalır — Duration sadece bırakma sonrası sönme süresini etkiler.

### Wheel (Çark)
Klavyenin merkezi etrafında dönen bir gökkuşağı.
- **Reverse Direction** — işaretlenince saat yönü yerine tersine döner.
- **Speed** — dönüş hızı.

### Lightning (Şimşek)
Klavyenin üstünden altına doğru düşen parlak bir "şimşek".
- **Speed** — şimşeğin ne kadar hızlı aşağı ineceği.
- **Smoothness** — şimşeğin arkasında bıraktığı kuyruğun ne kadar uzun ve yumuşak söneceği. Yüksek değerler daha uzun, daha yumuşak sönümlenen bir kuyruk oluşturur (ve tamamen kaybolması da buna bağlı olarak biraz daha uzun sürer).
- **Strike Width** — 1 ila 4 tuş genişliğinde.
- **Concurrent Strikes** — aynı anda kaç bağımsız şimşeğin düşebileceği (1-4). Tam sayı, kaydırıcının yanında canlı olarak gösterilir.

### Flame (Alev)
Sürekli gezinen/kayan bir ateş simülasyonu, alt kısımlar daha sıcak.
- **Flicker Speed** — ısı deseninin ne kadar hızlı değişip yeniden rastgeleleşeceği.
- **Smoothness** — renklerin bir durumdan diğerine ne kadar kademesiz geçeceği (ani sıçrama yerine).

### Rain (Yağmur)
Klavyeden aşağı düşen renkli damlalar, kısa bir kuyruk izi bırakarak.
- **Fall Speed** — damlaların ne kadar hızlı düşüp animasyonunu tamamlayacağı.
- **Density** — 1 ila 5 kademe; her kademe 3 eş zamanlı damla ekler (1. kademe = 3 damla, 5. kademe = 15 damla).
- **Color Count** — 1 ila 5. Bir sayıya tıklayarak paletteki renk sayısını belirle, sonra beliren her renk kutusuna tıklayıp rengini ata. Her damla, doğduğu anda paletten rastgele seçilen tek bir renkte kalır (yaşam süresi boyunca değişmez).

### Matrix
Style açılır kutusundan seçilebilen 4 farklı animasyon stili:
- **Diagonal Streaks** — rastgele çapraz açılarla klavye üzerinde akan renkli bantlar.
- **Vertical Columns** — klasik "dijital yağmur" görünümü, her sütunda düz aşağı akış.
- **Laser Sweep** — 2-3 keskin, hızlı yatay/dikey çizgi geçişi.
- **Ripple** — rastgele noktalardan dışa doğru genişleyen halkalar (su dalgası gibi).

Ayarlar:
- **Streak Speed** — genel animasyon hızı.
- **Density** — Diagonal Streaks ve Laser Sweep stillerini etkiler: aynı anda kaç çizginin aktif olacağı.
- **Color Count** — 1 ila 3, Rain'deki ile aynı tıkla-ata sistemi.

---

## 6. Sorun Giderme

**Açılışta "Could not open device" hatası.**
Kontrol Merkezi'nde aydınlatma hâlâ açık — orada kapat (Bölüm 1) ve tekrar dene.

**Klavye ışıkları hem bu uygulamaya hem üretici yazılımına tepki vermemeye başlıyor.**
Klavyenin gömülü kontrolcüsü kilitlenmiş — bu bilinen bir donanımsal tuhaflık, veri bozulması ya da kalıcı hasar değil. Kurtarmak için:
1. Laptobu tamamen kapat.
2. Şarj kablosunu çıkar.
3. Güç düğmesine yaklaşık 30 saniye bas.
4. Şarjı tekrar tak, laptobu aç.

**Bir modun renk ayar kutuları, başka bir mod seçiliyken de ekranda görünüyor.**
Bu, önceki sürümlerde bir hataydı ve düzeltildi; hâlâ görüyorsan en güncel derlemeyi kullandığından emin ol.

**Reactive modda numpad Enter ile ana Enter tuşu birlikte yanıyor.**
Bilinen bir sınırlama — Windows ikisini de aynı sanal tuş koduyla bildiriyor, uygulama şu an bunları ayırt edemiyor.

**Fn tuşu ya da Copilot tuşu Reactive modda hiç yanmıyor.**
Bu da beklenen bir durum — bu tuşlar Windows'un algılayabileceği standart bir sinyal göndermiyor.
