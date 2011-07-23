<?php
/**
 * $Id: NL_Translator_DOM.class.inc 7 2009-03-03 07:15:55Z NaDiN $
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

require_once(TARJ_DIR.'NL_Translator.class.inc');

$NODETYPES[XML_ELEMENT_NODE] = 'ELEMENT';
$NODETYPES[XML_TEXT_NODE] = 'TEXT';
$NODETYPES[XML_CDATA_SECTION_NODE] = 'CDATA';
$NODETYPES[XML_COMMENT_NODE] = 'COMMENT';
$NODETYPES[XML_DOCUMENT_TYPE_NODE] = 'DOCUMENT_TYPE';
$NODETYPES[XML_HTML_DOCUMENT_NODE] = 'HTML_DOCUMENT';

class NL_Translator_DOM extends NL_Translator {

	function NL_Translator_DOM() {
		parent::NL_Translator();
		$this->domEncodingFound = false;
		$this->elementToTranslate = array(
            	'body' => true,
            	'p' => true,
            	'a' => true,
            	'font' => true,
            	'span' => true,
            	'big' => true,
            	'small' => true,
            	'i' => true,
            	'b' => true,
            	'em' => true,
            	'strong' => true,
            	'td' => true,
            	'div' => true,
		);
	}

	function ReplaceTextNode(&$tokens, &$domNode) {
		$text = '';$word = '';
		$count = count($tokens);
		$parentNode = &$domNode->parentNode;
		for ($it = 0; $it < $count; $it++) {
			$token = $tokens[$it];

			if ($token[TOK_TYPE] == TTYP_WORD) {
				if (strlen($text)) {
					$parentNode->insertBefore($this->DOCUMENT->createTextNode($text), $domNode);
					$text = '';
				}
				$result = $this->TranslateToken($tokens, $it, $token, $word);
				if (is_array($word))
				{
					$word = '';
				}
				$spanNode = $this->DOCUMENT->createElement('span', $word);
				@$spanNode->setAttribute('title',$token[TOK_VAL]);
				$parentNode->insertBefore($spanNode, $domNode);
			} else {
				#$text .= ' $'.$token[TOK_VAL].'% ';
				$text .= $token[TOK_VAL];
			}
		}
		if (strlen($text)) {
			$parentNode->insertBefore($this->DOCUMENT->createTextNode($text), $domNode);
			$text = '';
		}
		$parentNode->removeChild($domNode);
	}

	function TranslateTextData(&$textNode) {
		$tokens = $this->AnalyzeText($textNode->data);
		$text = $this->GenerateText($tokens);
		$textNode->replaceData(0,strlen($textNode->data),$text);
	}

	function TranslateDOMText(&$domNode) {
		$parentNode = $domNode->parentNode;
		$validNode =
		($domNode->nodeType == XML_TEXT_NODE)
		&& ($parentNode->nodeType == XML_ELEMENT_NODE)
		&& isset($this->elementToTranslate[$parentNode->nodeName])
		&& $this->elementToTranslate[$parentNode->nodeName]
		;
		if ($validNode) {
			if ($this->annotation) {
				$tokens = $this->AnalyzeText($domNode->data);
				$this->ReplaceTextNode($tokens, $domNode);
			} else {
				$this->TranslateTextData($domNode);
			}
		}
		return $validNode;
	}

	function SanitizeDOMElement(&$domNode) {
		$nodeName = $domNode->nodeName;
			
		/* sanitize link elements */
		if (($nodeName == 'a') && ($link = $domNode->getAttribute('href'))) {
			$this->SanitizeLink($link);
			$domNode->setAttribute('href',$link);
		}
		else if (($nodeName == 'link') && ($link = $domNode->getAttribute('href'))) {
			$type = $domNode->getAttribute('type');
			if ($type == "text/css") {
				$this->SanitizeLink($link, false);
			} else {
				$this->SanitizeLink($link, false);
			}
			$domNode->setAttribute('href',$link);
		}
		else if (($nodeName == 'img' || $nodeName == 'embed' || $nodeName == 'script') && ($link = $domNode->getAttribute('src'))) {
			$this->SanitizeLink($link,false);
			$domNode->setAttribute('src',$link);
		}
		else if (($nodeName == 'param') && ($domNode->getAttribute('name') == 'movie') && ($link = $domNode->getAttribute('value'))) {
			$this->SanitizeLink($link,false);
			$domNode->setAttribute('value',$link);
		}
		else if (($nodeName == 'title')) {
			if ($domNode->firstChild && $domNode->firstChild->nodeType == XML_TEXT_NODE) {
				$annotation = $this->annotation;
				$this->annotation = false;
				$this->TranslateTextData($domNode->firstChild);
				$this->annotation = true;
			}
		}
		else if (($nodeName == 'style')) {
			#print ($domNode->firstChild->textContent);
			if ($domNode->firstChild && preg_match('/\@([Ii]mport|IMPORT)\s+\"([^\"]+)\"/', $domNode->firstChild->nodeValue, $match)) {
				$this->SanitizeLink($match[2]);
				$domNode->firstChild->nodeValue = '@import "'.$match[2].'";';
			}
		}
		else if (($nodeName == 'body')) {
			$nodeName = $domNode->nodeName;
			$this->bodyNode = $domNode;
		}
			
		/* fix encoding */
		if (($nodeName == 'meta') && (strtolower($domNode->getAttribute('http-equiv')) == 'content-type')) {
			$domNode->setAttribute('content', 'text/html; charset=utf-8');
			$this->domEncodingFound = true;
		}
	}

	function TreeWalking(&$domNode) {
		if (!$domNode) return;
		if ($domNode->nodeType == XML_ELEMENT_NODE) {
			$this->SanitizeDOMElement($domNode);
		} else {
			$this->TranslateDOMText($domNode);
		}
		$length = isset($domNode->childNodes) ? $domNode->childNodes->length : 0;

		/* Go to sub nodes */
		#for ($i=0;$i<$length;$i++) {
		$indent = '';
		for ($i=$length-1;$i>=0;$i--) {
			$indent .= "-";
			$childItem = $domNode->childNodes->item($i);
			$this->TreeWalking($childItem);
			$indent = substr($indent,1);
		}

		if (($domNode->nodeName == 'head') && !$this->domEncodingFound) {
			/* not found while sanitizing child elements */
			$metaNode = $this->DOCUMENT->createElement('meta');
			$metaNode->setAttribute('http-equiv', 'Content-Type');
			$metaNode->setAttribute('content', 'text/html; charset=utf-8');
			$domNode->appendChild($metaNode);
		}
	}

	function TreeWalkingDump(&$domNode, &$indent) {
		global $NODETYPES;

		if (!($nodeType = $NODETYPES[$domNode->nodeType])) {
			$nodeType = $domNode->nodeType;
		}

		if ($domNode->nodeType == XML_ELEMENT_NODE) {
			$type = "&lt;".$domNode->nodeName."&gt;";
		} else {
			$type = '';
		}
			
		if (isset($domNode->data)) {
			$text = $domNode->data;
			$text = " |$text|";
		} else {
			$text = '';
		}
			
		if ($domNode->hasChildNodes()) {
			$length = $domNode->childNodes->length;
		} else {
			$length = 0;
		}
		$lenInfo = " ChildNodes = $length";

		$this->walkDump .= "$indent$nodeType:$type$lenInfo$text\n";

		/* Go to sub nodes */
		for ($i=0;$i<$length;$i++) {
			#for ($i=$length-1;$i>=0;$i--) {
			$indent .= "-";
			$this->TreeWalkingDump($domNode->childNodes->item($i), $indent);
			$indent = substr($indent,1);
		}
	}

	function GetWalkDump() {
		return $this->walkDump;
	}

	function TranslateDOMTree(&$domTree) {
		$this->TimeMarkerStart('timeToParse');

		if (0)
		{
			$indent = '';
			$this->TreeWalkingDump($domTree, $indent);
			$output = "<pre>\n".$this->GetWalkDump()."</pre>";
			return ($output);
		}

		$this->DOCUMENT = &$domTree;
		$this->TreeWalking($domTree);

		$this->TimeToParse = $this->TimeElapsed('timeToParse');

		if ($this->siteMode) {
			$box = new NL_InputBox();
			$box->elements[] = $this->StatsBox();
			$inputBox = $box->InputBoxForSite($this->article, 'loka.php');

			if ($this->bodyNode)
			{
				$nodeName = $this->bodyNode->nodeName;
				$inputBoxNode = @DOMDocument::loadHTML($inputBox);
				$inputBoxNode = $inputBoxNode->childNodes->item(1)->firstChild->firstChild;
				$inputBoxNode = $domTree->importNode($inputBoxNode, 5);

				if ($this->bodyNode->firstChild) {
					$this->bodyNode->insertBefore($inputBoxNode, $this->bodyNode->firstChild);
				} else {
					$this->bodyNode->appendChild($inputBoxNode);
				}
				$inputBox = '';
			}

			if ($this->TermsTotalHit() > 0 && $this->TermsExistPercent() < 100.0) {
				ob_start();
				$box->PrintMessageBox("Kecap-kecap Nu Teu Aya dina Kamus:", $this->TermsNewList());
				$reportBox = ob_get_contents();
				ob_end_clean();
			}
		} else {
			$inputBox = '';
		}

		$domTree->formatOutput = true;
		$output = $inputBox . $domTree->saveHTML() . $reportBox;

		return $output;
	}

	function TranslateHTML($htmlText) {
		$this->TimeMarkerStart('timeToLoad');
		$domTree = @DOMDocument::loadHTML($htmlText);
		$this->TimeToLoad = $this->TimeElapsed('timeToLoad');
		return $this->TranslateDOMTree($domTree);
	}

	/**
	 * Translate the given $file. It can also be a URL.
	 */
	function TranslateHTMLFile($file) {
		$this->InitURLBase($file);
		#var_dump($this->urlbase);

		$this->TimeMarkerStart('timeToLoad');
		$domTree = new DOMDocument();
		error_reporting(E_ERROR);
		if (!$domTree->loadHTMLFile($file)) {
			print "Unable to load $file.";
			return;
		}
		error_reporting(E_ALL);
		$domTree->normalizeDocument();
		$this->TimeToLoad = $this->TimeElapsed('timeToLoad');

		return $this->TranslateDOMTree($domTree);
	}

};
?>
