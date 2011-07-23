<?php

define ('TARJ_DIR', '../inc/');
require_once (TARJ_DIR . 'NL_Translator_ID2SU.class.inc');

$kalimahs = array();

$kalimahs[] = array(
'Darmawati tertangkap tangan bersama anggota Komisi V Dewan Perwakilan Rakyat dari Fraksi Partai Amanat Nasional, Abdul Hadi Jamal.'
);
$kalimahs[] = array(
'Ketiga proyek itu telah ditender dan dimenangkan perusahaan yang terkait dengan Hontjo Kurniawan.',
);
$kalimahs[] = array(
'Sebagai jalan keluar, saat ini Ikapi tengah mendorong pemerintah untuk berani memborong semua buku sekolah dan membagikannya ke seluruh siswa secara gratis.',
);
$kalimahs[] = array(
'Dia menjabat sebagai Kepala Sekolah Menengah Atas Negeri Tiga Bandung.',
);
$kalimahs[] = array(
'Saya dapat pepaya.',
);
$kalimahs[] = array(
'Saya dapat menghitung.',
);

$nl = new NL_Translator_ID2SU();

print "<table>\n";
foreach ($kalimahs as $kalimah)
{
  $trans = $nl->TranslateText($kalimah[0]);
  print "<tr><td>$kalimah[0]</td><td>$trans</td></tr>\n";
}
print "</table>\n";

?>