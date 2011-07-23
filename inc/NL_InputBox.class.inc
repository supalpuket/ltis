<?php
/**
 * $Id: NL_InputBox.class.inc 7 2009-03-03 07:15:55Z NaDiN $
 *
 * Program Loka Tarjamah Otomatis Basa Indonésia - Basa Sunda
 *
 * Copyright (C) 2007 Dian Tresna Nugraha <dian.nugraha@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

define('_sspan_','<span style="color: #FF3300; font-family: Georgia; sans-serif;">');
define('_espan_','</span>');
define('_BETA_', _sspan_.'V&eacute;rsi B&eacute;ta'._espan_);
define('_CONTOTUMBU_', _sspan_.'Conto Tumbu: '._espan_);
define('_TITLETEXT_', 'Loka Tarjamah Otomatis Basa Indon&eacute;sia - Basa Sunda');
define('_COPYRIGHT_', '&copy; 2007-2009. Program diserat ku Dian Tresna Nugraha. Dipedar kalawan <a href="http://www.gnu.org/copyleft/gpl.html#TOC1">lis&eacute;nsi b&eacute;bas GNU V&eacute;rsi 2</a>');
define('_SOURCEDOC_', '<a href="./doc/tarjamah-20090304.zip">Kode sumberna (ZIP)</a> | <a href="./doc/LTIS-1.0.pdf">Dokum&eacute;ntasi (PDF)</a>');

define('TEXT', 1);
define('SITE', 2);

function NL_hash_implode($hash)
{
	$output = array();
	foreach ($hash as $key => $value)
	{
		$output[] = "$key=$value";
	}
	return implode($output, "&");
}

function NL_refine_request()
{
	mb_internal_encoding('UTF-8');
	if (!defined('_refine_request_called'))
	{
		define('_refine_request_called',1);
		global $_GET, $_POST, $_REQUEST, $_SERVER;
		if (isset($_GET['debug_port']) && ($_GET['debug_port'] == '11000')) {
			if (preg_match('/loka/',$_SERVER['PHP_SELF'])) {
				#$_REQUEST['article'] = 'http://diantn.sytes.net/QuranSE/?kata=2:1-&lang=id&lite=on';
				#$_REQUEST['article'] = 'd:/WebProjekte/tarjamah/test/qse_01.html';
				$_REQUEST['article'] = 'd:/WebProjekte/tarjamah/test/brword.html';
			} else {
				$_REQUEST['article'] = 'kegiatan PON XVII';
				$_REQUEST['article'] = 'saya tinggal di Eropa';
				$_REQUEST['article'] = 'Wali Kota Bandung Dada Rosada menerima penghargaan pembina terbaik dalam penerapan keselamatan dan kesehatan kerja (K3) tingkat Prov. Jawa Barat 2009. Penghargaan diberikan berdasarkan kemampuannya melakukan pembinaan penerapan program K3 di setiap perusahaan di Kota Bandung.';
			}
		}
		else if (isset($_POST['Konci']) && ($_POST['Konci'] == NL_Keyword()))
		{   /* Post from OUR form */

		}
		else if (count($_POST))
		{   /* Post from other's form */
			#$debug = true;

			$selfReferer = parse_url($_SERVER['HTTP_REFERER']);
			$validSelfReferer =
			($selfReferer['host'] == $_SERVER['HTTP_HOST'])
			&& ($selfReferer['path'] == $_SERVER['PHP_SELF'])
			&& isset($selfReferer['query']);
			if ($debug)
			{
				var_dump($selfReferer);
				print "<hr/>";
				print "$_SERVER[HTTP_HOST]$_SERVER[PHP_SELF]";
				print "<hr/>";
			}
			if ($validSelfReferer)
			{
				$refererQuery = $selfReferer['query'];
				$refererQuery = substr($refererQuery, 1+strpos($refererQuery, '='));
				if ($debug)
				{
					print $refererQuery;
					print "<hr/>";
				}
				$referer = parse_url($refererQuery);
				if ($debug)
				{
					var_dump($referer);
					print "<hr/>";
				}
				$query = array();
				if (isset($referer['query']))
				{
					$refererQuery = mb_split('&',$referer['query']);
					foreach ($refererQuery as $queryItem)
					{
						$queryItem = mb_split('=',$queryItem);
						$query[$queryItem[0]] = $queryItem[1];
					}
				}
				foreach ($_POST as $key => $value)
				{
					$query[$key] = $value;
				}
				if ($debug)
				{
					var_dump($query);
					print "<hr/>";
				}

				$_REQUEST['article'] = "$referer[scheme]://$referer[host]$referer[path]?".
				NL_hash_implode($query);

				$redirectLocation = "$selfReferer[scheme]://$_SERVER[HTTP_HOST]$_SERVER[PHP_SELF]"
				."?article=$_REQUEST[article]";

				header("Location: $redirectLocation\n");
			}

			if ($debug)
			{
				print "Redirect = $redirectLocation";
				print "<hr/>";
				print "Referer = " .$_SERVER['HTTP_REFERER'];
				print "<hr/>";
				print "Article = " . $_REQUEST['article'];
				print "<hr/>";
				die();
			}
		}
		else if (isset($_SERVER['QUERY_STRING']))
		{
			$_REQUEST['article'] = urldecode(substr($_SERVER['QUERY_STRING'], 1+strpos($_SERVER['QUERY_STRING'], '=')));
			$cpos = strrpos($_REQUEST['article'], '&Kintun');
			if ($cpos) {
				$_REQUEST['article'] = substr($_REQUEST['article'], 0, $cpos);
			}
			#print ($_REQUEST['article']);
		}
	}
}

function NL_Keyword()
{
	global $NL_KEYWORD;
	if (!isset($NL_KEYWORD))
	{
		$NL_KEYWORD = md5('Loka Tarjamah Indonesia Sunda');
	}
	return $NL_KEYWORD;
}

class NL_InputBox {
	function NL_InputBox() {
		$this->noHomeLink = false;
		$this->elements = array();
		$this->bodyStyle =
			"margin: auto;" .
		#"background: url(snow_ridge.png);" .
			"background: url(img/blue_type.png);" .
			"width: 800px;"
			;

			$this->titleStyle =
			"font-size: 20px;" .
			"line-height: 1.2;" .
			"margin: 0px;";

			$this->boxStyle =
			"text-align:left;" .
			"clear:both;" .
			"font-family: sans-serif;" .
			"font-size: 10pt;" .
			"border:3px solid #A199FF;" .
			"padding:3px;" .
			"margin:3px;" .
			"background: url(img/snow_ridge.png);"
			#"background-color:#FFFF99;"
			;

			$this->inputStyle =
			"font-family: monospace;" .
			"font-size: 9pt;" .
			"border:1px solid #A199FF;" .
			"background-color:#dddddd;"
			;

			$this->boxTitleStyle =
			"color: #996600;" .
			"font-size: 12pt;" .
			"font-weight: bold;"
			;
	}
	function PrintHeader() {
		print "<html>\n";
		print "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\n";
		print "<title>Loka Tarjamah Otomatis Indon&eacute;sia - Sunda (ku: Dian Tresna Nugraha)</title>\n";
		print "<body style=\"$this->bodyStyle;\">\n";
		print "<div style=\"$this->boxStyle;text-align: center;\">\n";
		print "<p style=\"$this->titleStyle\">". _TITLETEXT_."</p>\n";
		print _COPYRIGHT_."<br/>\n";
		print _SOURCEDOC_."\n";
	}

	function PrintFooter() {
		print "</div>\n";
		print "</body>\n";
		print "</html>\n";
	}

	function PrintHomeLink() {
		if (!$this->noHomeLink) {
			print '<a href="./">[KACA UTAMA]</a>';
		}
	}

	function SetNoHomeLink($stat)
	{
		$this->noHomeLink = $stat;
	}

	function PrintInputBoxForText($article, $actionLink) {
		print "<div style=\"$this->boxStyle\">\n";
		print $this->PrintHomeLink()."\n";
		print "<form method=\"post\" action=\"$actionLink\" style=\"margin:2px\">\n";
		print _BETA_." : Tarjamahan Indon&eacute;sia-Sunda sacara otomatis tina:<br/>\n";
		print "	<textarea style=\"$this->inputStyle\" name=\"article\" cols=\"80\" rows=\"16\">".(($article)? $article: "")."</textarea>\n";
		print "	<br/><input type=\"submit\" name=\"Kintun\" value=\"Kintun\"/><br/>\n";
		print $this->PrintKeyword();
		print "</form>\n";
		foreach ($this->elements as $element) {
			print $element;
		}
		print "</div>\n";
	}

	function PrintMessageBox($title,$text) {
		print "<div style=\"$this->boxStyle\">\n";
		print "<p style=\"$this->boxTitleStyle\">$title</p>\n";
		print $text."\n";
		print "</div>\n";
	}

	function InputBoxForSite($article, $actionLink) {
		ob_start();
		$this->PrintInputBoxForSite($article, $actionLink);
		$output = ob_get_contents();
		ob_end_clean();
		return $output;
	}

	function PrintKeyword()
	{
		$konci = NL_Keyword();
		print "	<input type=\"hidden\" name=\"Konci\" value=\"$konci\"/>\n";
	}

	function PrintInputBoxForSite($article, $actionLink) {
		print "<div style=\"$this->boxStyle\">\n";
		print $this->PrintHomeLink()."\n";
		print "<form method=\"get\" action=\"$actionLink\" style=\"margin:2px\">\n";
		print _BETA_." : Tarjamahan Indon&eacute;sia-Sunda sacara otomatis tina:<br/>\n";
		print "	<input style=\"$this->inputStyle\" type=\"text\" name=\"article\" size=\"80\" value=\"".(($article)? $article: "http://")."\" />\n";
		print "	<input type=\"submit\" name=\"Kintun\" value=\"Kintun\"/>\n";
		print $this->PrintKeyword();
		print "</form>\n";
		if ($article) {
			print " <a href=\"$article\" style=\"text-decoration:underline;\">[LOKA ASAL]</a>";
		} else {
			print $this->BookmarksList();
		}
		foreach ($this->elements as $element) {
			print $element;
		}
		print "</div>\n";
	}

	function InsertInputBoxForSite(&$domTree, &$bodyNode, $article, $actionLink) {
	}

	function InsertInputBoxForSiteX(&$domTree, &$bodyNode, $article, $actionLink) {
		$textInput = $domTree->createElement('input');
		$textInput->setAttribute('style', $this->inputStyle);
		$textInput->setAttribute('type', 'text');
		$textInput->setAttribute('name', 'article');
		$textInput->setAttribute('size','80');
		$textInput->setAttribute('value', ($article)? $article:'');
		$submitInput = $domTree->createElement('input');
		$submitInput->setAttribute('type', 'submit');
		$submitInput->setAttribute('name', 'Kintun');
		$submitInput->setAttribute('value', 'Kintun');
		$formNode = $domTree->createElement('form');
		$formNode->setAttribute('method','get');
		$formNode->setAttribute('action',$actionLink);
		$formNode->setAttribute('style','margin:2px');
		$formNode->appendChild($domTree->createTextNode(_BETA ." : Tarjamahan Indon&eacute;sia-Sunda sacara otomatis tina:"));
		$formNode->appendChild($domTree->createElement('br'));
		$formNode->appendChild($textInput);
		$formNode->appendChild($submitInput);
		#$formNode->appendChild($domTree->)
		$boxNode = $domTree->createElement('div');
		if ($bodyNode->length) {

		} else {
			$bodyNode->appendChild();
		}
	}

	function PrintInputBox($article, $actionLink, $mode = SITE) {
		if ($mode == TEXT) {
			$this->PrintInputBoxForText($article, $actionLink);
		} else {
			$this->PrintInputBoxForSite($article, $actionLink);
		}
	}

	function BookmarksList() {
		$korans = array (
		/*		'Kompas' => 'http://www.kompas.com', */
		'Republika' => 'http://www.republika.co.id/halaman/174',
		'PR' => 'http://www.pikiran-rakyat.com',
		'MediaIndo' => 'http://mediaindonesia.com/index.php',
                'Tempo' => 'http://www.tempointeraktif.com',
		/*    'QuranSE' => 'http://diantn.sytes.net/QuranSE/?kata=2:1-&lang=id&display=30&lite=on&istext=on&submit=1&page=1', */
		);
		$output = _CONTOTUMBU_;
		foreach ($korans as $key => $link) {
			$output .= " <a href=\"loka.php?article=$link\">[$key]</a>";
		}
		return $output;
	}
}
?>
