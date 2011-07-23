<?php
/**
 * $Id: NL_InputBox.class.inc 5 2009-02-24 21:58:49Z NaDiN $
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

require_once TARJ_DIR . 'NL_Translator_DOM.class.inc';

class NL_Translator_CSV extends NL_Translator_DOM {
    function NL_Translator_CSV() {
        parent::NL_Translator_DOM();

        $this->names = array();
        $this->dictWords = array();
        $this->phrases = array();
        $this->foreigns = array();

        $this->DictRead(TARJ_DIR . '../data/id_su_names.csv', $this->names);
        $this->DictRead(TARJ_DIR . '../data/id_su_entries.csv', $this->dictWords);
        $this->DictRead(TARJ_DIR . '../data/id_su_main.csv', $this->dictWords);
        $this->DictRead(TARJ_DIR . '../data/id_su_phrases.csv', $this->phrases);
        $this->DictRead(TARJ_DIR . '../data/id_su_foreign.csv', $this->foreigns);
        #var_dump($this->foreigns);die();
    }

    function DictRead($filekamus, &$targetList) {
        if (!file_exists($filekamus)) {
            print "Not exists: $filekamus<br/>\n";
            return;
        }
        $lines = file($filekamus);
         
        if (false !== mb_strpos($lines[0], ";")) {
            $separator = ';';
        }
        else if (false !== mb_strpos($lines[0], "\t")) {
            $separator = "\t";
        } else {
            $separator = ',';
        }

        $nLines = count($lines);
        error_reporting(E_ERROR);
        for ($it=1;$it<$nLines;$it++) {
            list($id, $valid, $su, $fungsi, $fungsi2, $konteks, $bentuk)
            = mb_split($separator, trim($lines[$it]));
            #= explode("\t",iconv("UTF-8","ISO-8859-1",$line));
            if ($valid) {
                if (!isset($targetList[$id])) {
                    $targetList[$id] = array();
                }
                 
                $id = trim(str_replace('"', '', $id));
                $su = trim(str_replace('"', '', $su));
                $fungsi = trim(str_replace('"', '', $fungsi));
                $fungsi2 = trim(str_replace('"', '', $fungsi2));
                $konteks = trim(str_replace('"', '', $konteks));
                if ($su == '') { $su = $id; }
                if ($fungsi2 == '') { $fungsi2 = 0; }
                $targetList[$id][$fungsi2] = array (DIC_TR => $su, DIC_FN => $fungsi, DIC_CTX => $konteks);
            }
        }
        error_reporting(E_ALL);
    }

    function &_DictWords($key) {
        if (!isset($this->dictWords[$key])) {
            $result = false;
        } else {
            $result = &$this->dictWords[$key];
        }
        return $result;
    }
    function _DictNames($key) {
        if (!isset($this->names[$key])) {
            return false;
        }
        return $this->names[$key];
    }
    function _DictPhrases($key) {
        if (!isset($this->phrases[$key])) {
            return false;
        }
        return $this->phrases[$key];
    }
    function _DictForeigns($key) {
        if (!isset($this->foreigns[$key])) {
            return false;
        }
        return $this->foreigns[$key];
    }

    function TermsNewDoSave() {
        $file = $this->TermsNewFilename();
        if (file_exists($file)) {
            $lines = file($file);
            foreach ($lines as $line) {
                if (trim($line) != '') {
                    list($word,$freq) = mb_split(';',str_replace('"','',$line));
                    $oldNewWords[trim($word)] = 0+trim($freq);
                }
            }
        } else {
            $oldNewWords = array();
        }
        foreach($this->newDict as $word => $freq) {
            if (isset($oldNewWords[$word])) {
                $oldNewWords[$word] += $this->newDict[$word];
            } else {
                $oldNewWords[$word] = $this->newDict[$word];
            }
        }
        arsort($oldNewWords);
        reset($oldNewWords);
        $fp = fopen($file, 'w+');
        if ($fp) {
            $fcontent = '';
            foreach ($oldNewWords as $word => $freq) {
                if ($word) {
                    $fcontent .= "$word;$freq\n";
                }
            }
            fwrite($fp, $fcontent);
        } else {
            print 'Unable to write '.$file;
        }
        @fclose($fp);
    }

}
?>