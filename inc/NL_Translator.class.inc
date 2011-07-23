<?php
/**
 * File: $Id: NL_Translator.class.inc 9 2009-03-03 22:37:27Z NaDiN $
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
require_once(TARJ_DIR.'NL_InputBox.class.inc');
require_once(TARJ_DIR.'NL_UrlSanitizer.class.inc');

define ('KSMOD_HTML', 1);
define ('KS_ALLCAPS', 0xA000);
define ('KS_UCFIRST', 0xB000);
define ('KS_LOWERCASE', 0xC000);

define ('TTYP_WORD', 0x1);
define ('TTYP_NEWLINE', 0x2);
define ('TTYP_TERMINAL', 0x3);
define ('TTYP_NONTERMINAL', 0x4);
define ('TTYP_SPACES', 0x5);
define ('TTYP_DIGIT', 0x6);
define ('TTYP_URL', 0x7);
define ('TTYP_QUOTES', 0x8);
define ('TTYP_LBRACKETS', 0x9);
define ('TTYP_RBRACKETS', 0xA);
define ('TTYP_IGNORE', 0xB);
define ('TTYP_INVALID', 0x0);

define ('STR_BEGIN', 0x1);
define ('STR_MIDDLE', 0x2);

define ('ID_HIT', 0x1);
define ('ID_EXISTS', 0x2);
define ('ID_NEW', 0x3);
define ('ID_TERM', 0x4);

define ('TOK_TYPE', 0x2);
define ('TOK_VAL', 0x1);
define ('TOK_POS', 0x3);
define ('TOK_UC', 0x4);
define ('TOK_DASH', 0x5);
define ('TOK_VAL1', 0x6);
define ('TOK_VAL2', 0x7);

define ('DIC_TERM',0x0);
define ('DIC_VLD',0x1);
define ('DIC_TR',0x2);
define ('DIC_FN',0x3);
define ('DIC_CTX',0x4);

define ('STYP_TITLE', 1);
define ('STYP_NORMAL', 2);

/**
 * NL_Translator
 *
 * This class provides basic NL translator functionalities:
 * - reading vocabularies from text file (.csv)
 * - statistics of terms
 * - statement analyzer
 */
class NL_Translator extends NL_UrlSanitizer{

	var $stat;
	var $mode;

	function NL_Translator() {
		NL_refine_request();
		$this->termStats = array();
		$this->newDict = array();
		$this->stats[ID_NEW] = 0;
		$this->stats[ID_EXISTS] = 0;
		$this->annotation = true;
	}

	function TermsNewDoSave() {
		die ('TermsNewDoSave() is not implemented');
	}

	function TermsExistListPrint() {
		arsort($this->termStats);
		reset($this->termStats);
		$output = '';
		foreach ($this->termStats as $key => $value) {
			$output .= "$value; $key<br/>";
		}
		return $output;
	}

	function TermsNewList() {
		arsort($this->newDict);
		reset($this->newDict);
		$output = '';
		foreach($this->newDict as $key => $value) {
			$output .= "$value; $key<br/>";
		}
		return $output;
	}

	function Input() {
		return implode('', $this->inputLines);
	}

	function Percent($val, $div) {
		return (sprintf("%0.3f", 100 * $val / $div));
	}

	function StatsInit() {
		$this->stats[ID_NEW] = 0;
		$this->stats[ID_EXISTS] = 0;
	}

	function StatsPrint() {
		print "Hit: ".($hit = $this->TermsTotalHit())."<br/>";
		print "Ada: ".$this->stats[ID_EXISTS]." (".$this->Percent($this->stats[ID_EXISTS], $hit)."%)<br/>";
		print "Baru: ".$this->stats[ID_NEW]." (".$this->Percent($this->stats[ID_NEW], $hit)."%)<br/>";
	}

	function StatsBox() {
		ob_start();
		$this->PrintSingleStat();
		$output = ob_get_contents();
		ob_end_clean();
		return $output;
	}

	function TermsTotalHit() {
		$hit = $this->stats[ID_EXISTS] + $this->stats[ID_NEW];
		#if ($hit <= 0) die ('here');
		return $hit;
	}

	function TermsExistHit($term) {
		$this->stats[ID_EXISTS]++;
		$this->termStats[$term]++;
	}

	function TermsExistPercent() {
		if ($this->TermsTotalHit())
		return (100.0 * $this->TermsExistCount()/$this->TermsTotalHit());
		return 0.0;
	}

	function TermsStatsBox($column = 7) {
		$termStats = &$this->termStats;
		#var_dump($termStats);
		arsort($termStats);
		reset($termStats);
		$box = '<table>';
		$i = 0;
		foreach ($termStats as $term => $stat) {
			if (($i%$column) == 0) {
				$box .= "\n<tr>";
			}
			$box .= "<td>$term ($stat)</td>";
			if ((($i+1)%$column) == 0) {
				$box .= "\n<tr>";
			}
			$i++;
		}
		while (($i%$column) != 0) {
			$box .= "<td></td>";
			$i++;
			if (($i%$column) == 0) {
				$box .= "</tr>";
			}
		}
		$box .= "</table>";
		return $box;
	}

	function TermsExistCount() {
		return $this->stats[ID_EXISTS];
	}

	function TermsNewRegister($word) {
		if (strlen($word) <= 3) return;
		$this->stats[ID_NEW]++;
		$this->wordfound = false;
		if (isset($this->newDict[$word])) {
			$this->newDict[$word]++;
		} else {
			$this->newDict[$word] = 1;
		}
	}

	function TermsNewSave() {
		if ($this->TermsExistPercent() > 40.0) {
			$this->TermsNewDoSave();
		}
	}

	function AnalyzeUcMode($word) {
		if (preg_match('/^([ÉA-Z])([ÉA-Z]+)?/', $word, $caps)) {
			if (isset($caps[2]) && $caps[2]) {
				return KS_ALLCAPS;
			}
			return KS_UCFIRST;
		}
		return KS_LOWERCASE;
	}

	function GenerateUcAll($word) {
		$word = strtoupper($word);
		$word = str_replace('é', 'É', $word);
		return $word;
	}

	function GenerateUcFirst($word) {
		/* DEBUG:
		 if (is_array($word))
		 {
			print 'NL_Translator.class.php::GenerateUcFirst()';
			$word = implode(' ', $word);
			}
			*/
		if (1) {
			if (preg_match('/^[a-z]/i', $word)) {
				$word = ucfirst($word);
			} else {
				$word = 'É'.substr($word, 2);
			}
		} else {
			$words = mb_split(' ', $word);
			$count = count($words);
			for($i=0;$i<$count;$i++) {
				$word = $words[$i];
				if (preg_match('/^[a-z]/i', $word)) {
					$word = ucfirst($word);
				} else {
					$word = 'É'.substr($word, 2);
				}
				$words[$i] = $word;
			}
			$word = implode(' ',$words);
		}
		return $word;
	}

	function SetHTMLHandler($phpScript)
	{
		$this->htmlHandler = $phpScript;
	}

	function SetAnnotation($bool) {
		$this->annotation = $bool;
	}

	function SetSiteMode($bool) {
		$this->siteMode = $bool;
	}

	function AnalyzeText($text) {
		$tokens = array();
		$position = STR_BEGIN;
		$text = htmlspecialchars_decode($text);
		$quotes = array ('&#147;', '&#148;');
		$text = str_replace($quotes, '"', $text);
		
		$wordCount = 0;
		$ucCount = 0;

		while (1) {
			if (preg_match('/^((\w\w\w\w?\w?)\:\/\/[\w\.\/\-\?\&\=\%\;\~]+)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[1]));
				$tokens[] = array (
				TOK_TYPE => TTYP_URL,
				TOK_VAL => $matches[1],
				TOK_POS => $position,
				);
			}
			else if (preg_match('/^([\{\[\(]+)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[1]));
				$tokens[] = array (
				TOK_TYPE => TTYP_LBRACKETS,
				TOK_VAL => $matches[1],
				TOK_POS => $position,
				);
			}
			else if (preg_match('/^([\}\]\)]+)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[1]));
				$tokens[] = array (
				TOK_TYPE => TTYP_RBRACKETS,
				TOK_VAL => $matches[1],
				TOK_POS => $position,
				);
			}
			else if (preg_match('/^([\‘\’\“\”\'\"\=]+)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[1]));
				$tokens[] = array (
				TOK_TYPE => TTYP_QUOTES,
				TOK_VAL => $matches[1],
				TOK_POS => $position,
				);
			}
			else if (preg_match('/^ *([\.\?\!\:\|]+)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[0]));
				$tokens[] = array (
				TOK_TYPE => TTYP_TERMINAL,
				TOK_VAL => $matches[1],
				TOK_POS => $position = STR_BEGIN,
				);
			}
			else if (preg_match('/^(\s+)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[1]));
				$tokens[] = array (
				TOK_TYPE => TTYP_SPACES,
				TOK_VAL => $matches[1],
				TOK_POS => $position,
				);
			}
			else if (preg_match('/^(\w+([0-9]*-)?[0-9]+|_+\w*)/u', $text, $matches))
			{
				$text = substr($text, strlen($matches[1]));
				$tokens[] = array(
				TOK_TYPE => TTYP_IGNORE,
				TOK_VAL => $matches[1],
				TOK_POS => $position,
				);
			}
			else if (preg_match('/^([0-9\-]+)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[1]));
				$tokens[] = array (
				TOK_TYPE => TTYP_DIGIT,
				TOK_VAL => $matches[1],
				TOK_POS => $position,
				);
				$position = STR_MIDDLE;
			}
			else if (preg_match('/^((((\&\#\w+\;)|\w)+)((\-)(((\&\#\w+\;)|\w)+))*)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[1]));
				$ucMode = $this->AnalyzeUcMode($matches[1]);
				if (isset($matches[6]) && $matches[6]) {
					$tokens[] = array (
					TOK_TYPE => TTYP_WORD,
					TOK_VAL => strtolower($matches[1]),
					TOK_POS => $position,
					TOK_DASH => true,
					TOK_VAL1 => strtolower($matches[2]),
					TOK_VAL2 => strtolower($matches[7]),
					TOK_UC => $ucMode
					);
				} else {
					$tokens[] = array (
					TOK_TYPE => TTYP_WORD,
					TOK_VAL => strtolower($matches[1]),
					TOK_POS => $position,
					TOK_UC => $ucMode
					);
				}
				$position = STR_MIDDLE;
				$wordCount++;
				if ($ucMode != KS_LOWERCASE) {
				    $ucCount++;
				}
			}
			else if (preg_match('/^(\S+)/u', $text, $matches)) {
				$text = substr($text, strlen($matches[1]));
				$tokens[] = array (
				TOK_TYPE => TTYP_INVALID,
				TOK_VAL => $matches[1],
				TOK_POS => $position,
				);
			}
			else {
				break;
			}
		}
		$this->sentenceType = ($ucCount == $wordCount) ? STYP_TITLE : STYP_NORMAL;
		return $tokens;
	}

	function TranslateToken($tokens, &$it, &$token, &$word) {
		die('Please inherit TranslateToken()');
		return false;
	}

	function GenerateText(&$tokens) {
		$text = '';$word = '';
		$count = count($tokens);
		for ($it = 0; $it < $count; $it++) {
			$token = $tokens[$it];

			if ($token[TOK_TYPE] == TTYP_WORD) {
				$result = $this->TranslateToken($tokens, $it, $token, $word);

				if ($result && $this->annotation) {
					$text .= "<span title=\"".$token[TOK_VAL]."\">".$word."</span>";
				} else {
					$text .= $word;
				}
			} else {
				#$text .= ' $'.$token[TOK_VAL].'% ';
				$text .= $token[TOK_VAL];
			}
		}
		return $text;
	}

	function TranslateText($text) {
		$tokens = $this->AnalyzeText($text);
		#var_dump($tokens);die();
		$translatedText = $this->GenerateText($tokens);
		return $translatedText;
	}

	function TimeMarkerStart($id) {
		$this->sTime[$id] = gettimeofday(true);
	}

	function TimeElapsed($id) {
		return gettimeofday(true) - $this->sTime[$id];
	}

	/*
	 function TranslateHTMLFile($file) {
	 print ("IMPLEMENTED IN INHERITING CLASS");
	 }
	 */
	/**
	 * TranslateFile returns translated version of $inputFile.
	 * It is assumed that different line contains text in different context.
	 *
	 * @param $inputFile
	 */

	function TranslateFile($inputFile) {
		$this->StatsInit();

		$translation = $token = $line = $_bline = '';

		if (!file_exists($inputFile)) {
			$output = "Could not open file: $fileartikel\n";

		} else {
			if (preg_match('/^(http\:\/\/.*|.*\.(html|htm))$/i', $inputFile)) {
				if (isset($this->htmlHandler))
				{
					$redirect = "$this->htmlHandler?article=".$inputFile;
					header("Location: $redirect");
					die("REDIRECT: $redirect");
				}
				else
				{
					$this->TranslateHTMLFile($inputFile);
				}
			} else {
				$this->inputLines = file($inputFile);
				$this->mode = 0;

				$output = '';
				foreach($this->inputLines as $text) {
					$output .= $this->TranslateText($text);
				}
				return $output;
			}
		}
	}

	/**
	 * TranslateURL passes the URL to the proper handler based on the content-type.
	 * Limitation: currently it assumes only HTML.
	 */
	function TranslateURL($url) {
		return $this->TranslateHTMLFile($url);
	}

	function HandleRequestText() {
		$article = stripslashes(trim($_REQUEST['article']));

		if ($article != '') {
			$this->TimeToLoad = 0;
			$this->TimeMarkerStart('TimeToParse');
			$this->SetAnnotation(true);
			$this->translation = $this->TranslateText($article);
			$this->TimeToParse = $this->TimeElapsed('TimeToParse');
		}

		$box = new NL_InputBox;

		$box->PrintHeader();
		print "<br/><br/>";
		$box->PrintInputBox($article,'teks.php',TEXT);
		if ($article) {
			print "<br/>";
			$box->PrintMessageBox("Tarjamahan:", nl2br($this->translation));

			print "<br/>";
			if ($this->TermsExistPercent() < 100.0) {
				$box->PrintMessageBox("Kecap-kecap Nu Teu Aya dina Kamus:", $this->TermsNewList());
			}
			$this->TermsNewSave();
		}
		$box->PrintFooter();
	}

	function HandleRequestSite() {

		if ($article = $_REQUEST['article']) {
			if (file_exists($article)) {
				$article = str_replace('\\\\','/',$article);
			}

			$this->SetAnnotation(true);
			$this->SetSiteMode(true);
			print $this->TranslateURL($this->article = $article);
			$this->TermsNewSave();

		} else {
			$box = new NL_InputBox;
			$box->PrintInputBox($article,'loka.php',SITE);
		}
	}
}
?>
