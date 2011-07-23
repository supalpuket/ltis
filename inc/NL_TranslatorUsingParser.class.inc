<?php
/**
 * $Id: NL_TranslatorUsingParser.class.inc 3 2009-02-22 22:57:27Z NaDiN $
 *
 * Program Loka Tarjamah Otomatis Basa Indonésia - Basa Sunda
 * Vérsi: 0.1
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

require_once(TARJ_DIR.'htmlparser.inc');
require_once(TARJ_DIR.'NL_Translator.class.inc');

/**
 * NL_TranslatorUsingParser
 */
class NL_TranslatorUsingParser extends NL_Translator {
     
    function NL_TranslatorUsingParser() {
        parent::NL_Translator();
        $this->termStats = array();
        $this->names = array();
        $this->dictWords = array();
        $this->phrases = array();
        $this->newDict = array();
    }

    function SanitizeIfContainsLink(&$parser) {
        if ($parser->iNodeName == 'a' && isset($parser->iNodeAttributes['href'])) {
            $this->SanitizeLink($parser->iNodeAttributes['href']);
             
        }
        else if ($parser->iNodeName == 'link' && isset($parser->iNodeAttributes['href'])) {
            $this->SanitizeLink($parser->iNodeAttributes['href'], false);
             
        }
        else if ($parser->iNodeName == 'img') {
            $this->SanitizeLink($parser->iNodeAttributes['src'], false);
             
        }
        else if (($parser->iNodeName == 'param') && ($parser->iNodeAttributes['name'] == 'movie')) {
            $this->SanitizeLink($parser->iNodeAttributes['value'], false);
             
        }
        else if ($parser->iNodeName == 'embed') {
            $this->SanitizeLink($parser->iNodeAttributes['src'], false);
             
        }
    }

    /**
     * Translate the given $file. It can also be a URL.
     */
    function TranslateHTMLFile($file) {
        $this->InitURLBase($file);
        #var_dump($this->urlbase);

        $this->TimeMarkerStart('timeToLoad');
        $parser = HtmlParser_ForURL($file);
        $this->TimeToLoad = $this->TimeElapsed('timeToLoad');

        if (!$parser) {
            return;
        }

        $this->TimeMarkerStart('timeToParse');

        $prevNodeName = '';
        $prevNodeType = 0;
        $translateStart = false;
        $output = '';

        while ($parser->parse()) {
            if ($parser->iNodeType == NODE_TYPE_ELEMENT) {
                if ($parser->iNodeName == 'meta' && (strtolower($parser->iNodeAttributes['http-equiv']) == 'content-type')) {
                    $parser->iNodeAttributes['content'] = 'text/html; charset=utf-8';
                }
                else if ($parser->iNodeName == 'body') {
                    $translateStart = true;
                }
                else {
                    $this->SanitizeIfContainsLink($parser);
                }

                if ($parser->iNodeName == 'title') {
                    $parser->parse(); // advance to text item
                    $this->SetAnnotation(false);
                    $titletext = $this->TranslateText($parser->iNodeValue);
                    $this->SetAnnotation(true);
                    $output .= "<title>".$titletext;
                }
                else if (!$this->textOnly) {
                    $output .= "<$parser->iNodeName";
                    foreach ($parser->iNodeAttributes as $key => $value) {
                        $output .= " $key=\"$value\"";
                    }
                    $output .= ">";
                }
            }
            else if ($parser->iNodeType == NODE_TYPE_COMMENT) {
                #$output .= "<!--\n";
            }
            else if ($parser->iNodeType == NODE_TYPE_ENDCOMMENT) {
                #$output .= "-->";
            }
            else if ($parser->iNodeType == NODE_TYPE_ENDELEMENT) {
                if (!$this->textOnly) {
                    $output .= "</$parser->iNodeName>";
                }
            }
            else if ($translateStart && ($prevNodeType == NODE_TYPE_ELEMENT || $prevNodeType == NODE_TYPE_ENDELEMENT) && ($parser->iNodeType == NODE_TYPE_TEXT)) {
                switch($prevNodeName) {
                    case 'script':
                        break;
                    default:
                        $parser->iNodeValue = $this->TranslateText($parser->iNodeValue);
                        break;
                }
                $output .= $parser->iNodeValue;
                #$dump = false;
            }
            else {
                if (!$this->textOnly) {
                    $output .= $parser->iNodeValue;
                }
            }
             
            /*
             if ($translateStart) {
             var_dump($parser->iNodeName);
             var_dump($parser->iNodeType);
             var_dump($parser->iNodeValue);
             var_dump($parser->iNodeAttributes);
             echo "-----------------------------------\r\n";
             }
             */
            $prevNodeName = $parser->iNodeName;
            $prevNodeType = $parser->iNodeType;
        }

        $this->TimeToParse = $this->TimeElapsed('timeToParse');

        return $output;
        #echo "</pre>";
    }
}
?>
