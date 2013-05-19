// ObjectDetecion.cpp File
// Nesne algilama dosyasi

// Basliyoruz hadi

/********/
//OpenCV KÜTÜPHANELERI BASLANGIÇ
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
//OpenCV KÜTÜPHANELERI BITIS

#include <stdio.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

	/* kullanilacak olan boyut 640*480. Ideal olanin bu olduðu söyleniyo
		Bu bütün platformlar için ayni boyut demektir.
		Webcam 640*480 ebatlarina ayarlanmistir umarim :)
		Laptop dahili kameralarinda belki görüntü almada problem yaþabilir.
		Bu sorunun yasanmamasi için görüntü alma isleminin yüksek çözünürlüklü kameralarca yapilmasini öneririm
	*/
	CvSize boyut = cvSize(640,480);

	/*
		CvCapture ile video görüntüsünden yani stream dedigimiz akistan bilgi almak için burayi çagirdik
		Burayi daha sonra kullanacagiz
	*/
	CvCapture* p_camResimAl;

	/*
		Bu pointer'ı ilk giris için aldik yani kameraya gelen görüntüyü burada isliyoruz
	*/
	IplImage* p_orjinalResim;

	/*
		Bu kisimda artik resim islenmis durumda
		Yani webcam'den bi yukaridaki olayda goruntuyu saf haliyle almistik.
		Burada goruntu islenmis oldu ve saf hali artik yok
	*/
	IplImage* p_islenmisResim;
	// IPL dedigimiz sey Intel Image Processing Library'nin kisaltilmis halidir.
	// OpenCV 1.x'de kullanilmistir..
	///////////////////////////

	/*
		Burada cvHoughCircles() icin gerekli olan depolama degiskenini tanittik
	*/
	CvMemStorage* p_strDepolama;

	/*
		OpenCV dizisi icin olusturulan bi isaretci burasi
		Geri donusu cvHoughCircles() icin olacaktir
		Ayrica butun cevreleri kapsar, icerir..
		Çagirim söyle yapilir;
		cvGetSeqElem(p_dairelerDizisi, i) gibi 3 elemanli bi dizi de dondurur.
		Bi sonraki degiskende de anlasilacaktir oraya bakalim
	*/
	CvSeq* p_dairelerDizisi;

	/*
		3 elemanli dizi float bi isaretçi olacak bu yani pointer. Olayi açiklayacak olursak
		[0] => burada ilk indexte yapilacak olan olay objenin X pozisyonunu algilayacak
		[1] => burada yani ikinci indexte yapilacak olan olay da objenin y pozisyonunu algilamak
		[2] => son yani 3. olayda ise belirtilen nesnenin ya da objenin yari çapini algilama islemi yer alacak
	*/
	float* p_XYyaricap;

	int i; // döngü sayaci :)

	/*
		ESC icin klavyeden deger gelip gelmedigini kontrol edecegiz
	*/
	char escKarakterKontrolu;

	/*
		Burada 0 sayisi 1. kamerayi isaret eder. Buradaki sayi degisebilir. Artabilir.
		Söyle diyeyim eger 1'den fazla kameraya sahipseniz bu dedigim olabilir.
	*/
	p_camResimAl = cvCaptureFromCAM(0);



	if(p_camResimAl == NULL) { // Eger ki yakalama basarisiz olursa
        printf("Webcam'den resim alma islemi basarisiz oldu");
        getchar(); // Eger windows kullanan varsa diye burasi :)
        return(-1); // Programdan çıkış yapıyoruz
	}

	// Bu kısımda 2 pencere tanımlıyoruz. Birisi bilindiği gibi siyah beyaz
	// bir diğeri de bize tam zamanlı orjinal görüntü verecek
	// Pencere basligini belirttik ve otomatik olacak sekilde boyutlandirma ozelligi kattik
	cvNamedWindow("Orjinal Goruntu", CV_WINDOW_AUTOSIZE);

	// Bu kisimda ise islenecek goruntu icin bi pencere olusturuyoruz.
	// Ve yine otomatik boyutlandirma
	// Yuvarlaklari algilamak icin de zaten burayi kullaniyoruz
	cvNamedWindow("Islenmis Resim", CV_WINDOW_AUTOSIZE);

	// Yeni bi resim dosyasi olusturcagiz. Boyutlari da en basta belirlemistik zaten.
	// Bu IPL_DEPTH_8U olayi da resmin kac bitte bi derinlige sahip oldugunu belirtiyo.
	// Biz burada resmin 8 bit renk derinliginde olacagini belirttik.
	// 1 Kanalda calisacagimizi belirttik. Eger renkli bi goruntu olsaydi bu 1 yerine 3 olacakti
	p_islenmisResim = cvCreateImage(boyut,IPL_DEPTH_8U,1);

	// Her kare için belirtiyoruz
	while(1) {
        p_orjinalResim = cvQueryFrame(p_camResimAl); // Burada diyoruz ki kameradan bize kare al.

        // Eğer kare alma işlemi başarısız olursa diye bi işlem yürütmek gerekiyo
        if(p_orjinalResim == NULL) {
            printf("Kare alma islemi basarisiz oldu");
             // Windows kullananlar icin bu kaynak paylasilacak buyuk ihtimal bu hata onlara gelince
             // Ekran acilip kapanacaktir.
            getchar();

            break;

        }

        // Girdi fonksiyonu
        // Burada renklendirmeden bahsettik.
        // En dusuk filtre degeri yani bir de bunun en yuksek deger tasiyani var bu fonksiyonda
        // Maksimum deger tasiyani zaten 3. indekste gorulen 256'lık değer başlangıcıdır
        // Son olarak da çıktı fonksiyonu devreye giriyor özetleyecek olursak
        // Girdi fonksiyonu kameradan gelen orjinal görüntüyü alt ve üst değerleri belirtilmiş şekilde işler
        cvInRangeS(p_orjinalResim,CV_RGB(175,0,0), CV_RGB(256,100,100),p_islenmisResim);

        // cvHoughCircles() icin gerekli olan depolam alanini burada ayarladik
        p_strDepolama = cvCreateMemStorage(0);


        // Girdi ve çıktı fonksiyonları ile devam işlemini yaptık
        // Gaussian filtresini de burada kullanima actik
        // Buradan gaussian hakkında bilgi alınabilir: http://www.bulentsiyah.com/goruntu-filtreleme-uygulamalari-ve-amaclari-matlab/
        // Yumuşamada kullanılacak yükseklik ve genişlik değerleri de burada
        cvSmooth(p_islenmisResim,p_islenmisResim,CV_GAUSSIAN,9,9);

        // Diziyi islenmis resim ve etrafindaki cemberlerle dolduruyoruz
        // Girişteki görüntü renksiz olmalı giriş fonksiyonu girdi resmi burası orjinal hali
        // Renksiz kısım burası yani
        // Yapilan sey bellk depolamak icin bi fonksiyon uretmekti yapildi.
        // Bu gradient olayinda ise cemberleri algilayacagiz
        // Resim kac boyutlu? 2
        // Çevrelerin merkezleri ile aralarındaki mesafenin tesbiti burada yapılıyo: p_islenmisResim->height / 4
        // 100: Canny kenar çıkartma algoritmasında yüksek eşiği belirtir
        // 50: Canny kenar çıkartma algoritmasında düşük eşiği belirtir
        // Dairenin en az yarı çapını pixel pixel hesapladık :)
        // Tam tersi olarak da en yuksek yari capi ayni sekilde aldik bitti lan burasi sonunda
        // Karnim acikti simdi kokorec olsa da yesek midye falan
        p_dairelerDizisi = cvHoughCircles(p_islenmisResim, p_strDepolama, CV_HOUGH_GRADIENT, 2, p_islenmisResim->height / 4,
        100,50,10,400);

        // Aç olanlar buraya bakmaasın :(
        // Kokoreçli for
        for(i = 0; i < p_dairelerDizisi->total; i++) {
            // Her bi element için bi çember atadık.
            // Nesne algilama burada yapiliyo
            // X ve Y'nin yari capini ayarladik simdi. Daireler dizisi i kadar
            p_XYyaricap = (float*)cvGetSeqElem(p_dairelerDizisi, i);

            // Daire'nin ortasında X'in konumu,
            // Daire'nin ortasında Y'nin konumu
            // Çember'in yarı çapı
            printf("Top pozisyonu: X = %f , Y = %f , R = %f", p_XYyaricap[0], p_XYyaricap[1], p_XYyaricap[2]);

            // şimdi yeşil bi çember çizeceğiz küçük olacak
            // Bu çember algılanan nesnenin merkezini belirtecek
            // Bu çizim işlemini orjinal resim üzerinde uygulamaya koyacağız
            // yarı çapı 3 pixel olan bi çember
            cvCircle(p_orjinalResim,cvPoint(cvRound(p_XYyaricap[0]), cvRound(p_XYyaricap[1])),3,
            CV_RGB(0,255,0),CV_FILLED);

            // Şimdi bir de nesnenin etrafında kırmızı bi çember çizelim
            cvCircle(p_orjinalResim,cvPoint(cvRound(p_XYyaricap[0]), cvRound(p_XYyaricap[1])), cvRound(p_XYyaricap[2]),
            CV_RGB(255,0,0), 3);
        } // FOR BİTİŞİ

        cvShowImage("Orjinal Resim", p_orjinalResim);
        cvShowImage("Islenmis Resim", p_islenmisResim);

        cvReleaseMemStorage(&p_strDepolama);

        escKarakterKontrolu = cvWaitKey(10);
        if(escKarakterKontrolu == 27) break; //ASCII TABLOSUNDA ESC 27. KARAKTERE DENK GELIR

	} // WHİLE BİTİŞİ

	cvReleaseCapture(&p_camResimAl);

	cvDestroyWindow("Orjinal");
	cvDestroyWindow("Islenmis");

	return(0);

}

