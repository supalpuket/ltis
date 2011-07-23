<?php
/**
 * $Id: NL_Translator_ID2SU.class.inc 9 2009-03-03 22:37:27Z NaDiN $
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

#require_once('NL_TranslatorUsingParser.class.inc');

/*
 require_once(TARJ_DIR . 'NL_Translator_MYSQL.class.inc');

 class NL_Translator_ID2SU extends NL_Translator_MYSQL {

 function NL_Translator_ID2SU() {
 parent::NL_Translator_MYSQL();
 }
 */
require_once(TARJ_DIR . 'NL_Translator_CSV.class.inc');

class NL_Translator_ID2SU extends NL_Translator_CSV {

    function NL_Translator_ID2SU() {
        parent::NL_Translator_CSV();
    }

    function TermsNewTableName() {return 'tarj_id_su_new';}

    function TermsNewFilename() {return TARJ_DIR . '../data/id_su_new.csv';}

    function PrintSingleStat() {
        print sprintf (" [Dipulut: %0.3f detik]", $this->TimeToLoad);
        print sprintf (" [Ditarjamahkeun: %0.3f detik]", $this->TimeToParse);
        print sprintf (" [Tarjamahan: %0.3f %% ]", $this->TermsExistPercent());
    }

    function TranslateDir($dir) {
        if (!is_dir($dir)) return "Not a directory $dir\n";
         
        $files = array();
         
        $hd = opendir($dir);
        while ($file = readdir($hd)) {
            if (preg_match('/\.(wp|htm|html)$/i', $file)) {
                $files[] = array ('name' => "$dir/$file");
            }
        }
         
        $stats = array();
        $count = count ($files);
         
        for ($i = 0; $i < $count; $i++) {
            $this->TranslateFile($files[$i]['name']);
            $files[$i]['stats'] = $this->stats;
        }
         
        $output = "<table>";
        $output .= "<tr><th>Sumber</th>";
        $output .= "<th>Hit</th>";
        $output .= "<th>Aya</th>";
        $output .= "<th>Weuteuh</th>";
        $output .= "<th>Aya(%)</th>";
        $output .= "<th>Weuteuh(%)</th>";
        $output .= "</tr>\n";
         
        $sum_exists = 0;
        $sum_new = 0;
        $sum_hits = 0;
         
        foreach ($files as $file) {
            $count_exists = $file['stats'][ID_EXISTS];
            $count_new = $file['stats'][ID_NEW];
            $count_hits = $count_new + $count_exists;

            $sum_exists += $count_exists;
            $sum_hits += $count_hits;
            $sum_new += $count_new;

            $output .= "<tr><td><a href=\"$PHP_SELF?article=$file[name]\">$file[name]</a></td>";
            $output .= "<td>".$count_hits."</td>";
            $output .= "<td>".$count_exists."</td>";
            $output .= "<td>".$count_new."</td>";
            $output .= "<td>".$this->Percent($count_exists, $count_hits)."</td>";
            $output .= "<td>".$this->Percent($count_new, $count_hits)."</td>";
            $output .= "</tr>\n";

        }
        $percent_exists = sprintf ("%0.2f", 100 * $sum_exists / $sum_hits);
        $percent_new = sprintf ("%0.2f", 100 * $sum_new / $sum_hits);
        $output .= "<tr><td>TOTAL</td><td>$sum_hits</td><td>$sum_exists</td><td>$sum_new</td><td>$percent_exists</td><td>$percent_new</td></tr>\n";
        $output .= "</table>";
         
        return $output;
    }

    function m_KataUlang(&$token, &$word) {
        $match = $token[TOK_DASH]
        && ($token[TOK_VAL1] == $token[TOK_VAL2])
        && ($record = $this->_DictWords($token[TOK_VAL1]))
        ;
        if ($match) {
            $word = $record[0][DIC_TR].'-'.$record[0][DIC_TR];
            $this->TermsExistHit($word);
            return true;
        }
        return false;
    }

    /* baik ... atau|maupun ... */
    function m_baik_atau($it, &$tokens, &$word) {
        $token = $tokens[$it];
        if ($token[TOK_VAL] == 'baik') {
            do {
                $it++;
                $TTYP_i = $tokens[$it];
                if (!$TTYP_i || $TTYP_i[TOK_TYPE] == TTYP_TERMINAL) {
                    return $this->eitherOrActive = false;
                }
                if ($TTYP_i[TOK_VAL] == 'atau' || $TTYP_i[TOK_VAL] == 'maupun') {
                    $word = 'boh';
                    $this->TermsExistHit($word);
                    return ($this->eitherOrActive = true);
                }
            }
            while (1);
        }
        else if ($this->eitherOrActive && ($token[TOK_VAL] == 'atau' || $token[TOK_VAL] == 'maupun')) {
            $word = 'boh';
            $this->eitherOrActive = false;
            $this->TermsExistHit($word);
            return true;
        } else {
            return false;
        }
    }

    function m_Frase(&$it, &$token, &$token2, &$word) {
        $phrase = $token[TOK_VAL]." ".$token2[TOK_VAL];
         
        if ($record = $this->_DictPhrases($phrase)) {
            $it += 2;
            $token[TOK_VAL] = $phrase;
            $word = $record[0][DIC_TR];
            $this->TermsExistHit($phrase);
            return true;
        }
        else if ($record = $this->_DictPhrases($token[TOK_VAL])){
            $word = $record[0][DIC_TR];
            $this->TermsExistHit($word);
            return true;
        }

        return false;

    }

    function m_NamaIstilah(&$token, &$token_2, &$word) {
        #var_dump($this->foreigns); die();
        #if ($token[TOK_UC] == KS_UCFIRST && $record = $this->_DictNames($token[TOK_VAL])) {
        if ($record = $this->_DictNames($token[TOK_VAL])) {
            #print $token[TOK_VAL];
            if ($record[0][DIC_TR] != '') {
                $word = $record[0][DIC_TR];
            } else {
                $word = $token[TOK_VAL];
            }
            $this->TermsExistHit($word);
            return true;
        } else if ($this->_DictForeigns($token[TOK_VAL])){
            $word = $token[TOK_VAL];
            return true;

        } /*else if (($this->sentenceType == STYP_NORMAL) && ($token[TOK_UC] == KS_UCFIRST) && isset($token_2)) {
        $word = $token[TOK_VAL];
        return true;
        }
        */
        return false;
    }

    /* ... 'terlalu' +ajektif... */
    function m_terlalu_Ajektif(&$it, &$token, &$token2, &$word) {
        if ($token[TOK_VAL] == 'terlalu') {
            $it += 2;
            $this->TermsExistHit('teuing');
            if ($record = $this->_DictWords($token2[TOK_VAL])) {
                $word = $record[0][DIC_TR]." teuing";
                $this->TermsExistHit($record[0][DIC_TR]);
            } else {
                $word = $token2['val']." teuing";
                $this->TermsNewRegister($token2['val']);
            }
            return true;
        }
        return false;
    }

    /* ... 'paling' +ajektif... */
    function m_Superlatif(&$it, &$token, &$token2, &$word) {
        $match = ($token[TOK_VAL] == 'paling')
        && ($record = $this->_DictWords($token2[TOK_VAL]))
        && ($record[0][DIC_FN] == 'a');
        if ($match) {
            $word = "pang".$record[0][DIC_TR]."na";
            $this->TermsExistHit('pang');
            $this->TermsExistHit($record[0][DIC_TR]);
            $it += 2;
        }
        return $match;
    }

    /* ... 'masih' +ajektif... */
    function m_masih_Ajektif (&$it, &$token, &$token2, &$word) {
        $match = ($token[TOK_VAL] == 'masih')
        && ($record = $this->_DictWords($token2[TOK_VAL]))
        && ($record[0][DIC_FN] == 'a');
        if ($match) {
            $word = $record[0][DIC_TR]." kénéh";
            $this->TermsExistHit('kénéh');
            $this->TermsExistHit($record[0][DIC_TR]);
            $it += 2;
            return true;
        }
        return false;
    }

    /* ... 'kembali' +verb... */
    function m_kembali_Verba (&$it, &$token, &$token2, &$word) {
        $match = ($token[TOK_VAL] == 'kembali')
        && ($record = $this->_DictWords($token2[TOK_VAL]))
        && (($record[0][DIC_FN] == 'v') || ($record[0][DIC_FN] == 'vp'));
        if ($match) {
            $word = $record[0][DIC_TR]. " deui";
            $this->TermsExistHit('deui');
            $this->TermsExistHit($record[0][DIC_TR]);
            $it += 2;
            return true;
        }
        return false;
    }

    /* ... 'kembali' +ajektif ... */

    /* ... 'orang' ... */
    function m_orang(&$token, &$token_2, $token2, &$word) {
        $match = $token[TOK_VAL] == 'orang';
        if ($match) {
            if ($token2[TOK_UC] == KS_UCFIRST) {
                /* followed by name, e.g place */
                $word = 'urang';
            }
            else if (($record = $this->_DictWords($token_2[TOK_VAL])) && ($record[0][DIC_FN] == 'b')) {
                $word = 'urang';
            }
            else {
                $word = 'jelema';
            }
            $this->TermsExistHit($word);
        }
        return $match;
    }

    /* ... 'baru' ... */
    function m_baru(&$token, &$token_2, &$token2, &$word) {
        $match = $token[TOK_VAL] == 'baru';
        if ($match) {
            $record = $this->_DictWords($token2[TOK_VAL]);
            $token2_func = $record[0][DIC_FN];
            #var_dump($token);die('here');
            $match = ($token2_func == 'v')
            || ($token2_func == 'vp')
            || ($token2_func == 'm')
            || ($token2_func == 'b')
            || ($record[0][DIC_CTX] == 'waktu')
            || ($token2[TOK_TYPE] == TTYP_DIGIT)
            || ($token_2[TOK_TYPE] == TTYPE_TERMINAL);
            if ($match) {
                $word = 'kakara';
            } else {
                $word = 'anyar';
            }
            $this->TermsExistHit($word);
        }
        return $match;
    }

    /* ... 'dengan' ... */
    function m_dengan(&$token, &$token2, &$output) {

        if ($token[TOK_VAL] == 'dengan') {

            $data = $this->_DictWords($token2[TOK_VAL]);

            switch ($data[DIC_FN])
            {
                case 'n': /* nomina */
                    if ($record[0][DIC_CTX] == 'alat') {
                        $output = 'maké';
                    } else {
                        $output = 'jeung';
                    }
                    break;

                case 'pn': /* pronomina */
                case 'b': /* bilangan */
                case 'm': /* modal */
                    $output = 'jeung'; break;

                case 'v': /* verba */
                    $output = 'kalawan'; break;

                default: /* (salian eta) */
                    if ($token2[TOK_UC] == KS_UCFIRST)
                    $output = 'jeung';
                    else
                    $output = 'kalawan';
                    break;
            }
            $this->TermsExistHit($word);

            return true;
        }

        return false;
    }
    /* ... pukul +digit ... */
    function m_pukul_Digit(&$token, &$token2, &$word) {
        if ($token[TOK_VAL] == 'pukul' && $token2[TOK_TYPE] == TTYP_DIGIT) {
            $word = 'jam';
            $this->TermsExistHit($word);
            return true;
        }
        return false;
    }

    /* ... hari ... */
    function m_hari(&$token, &$token_2, &$token2, &$word) {
        if ($token[TOK_VAL] == 'hari') {

            $record = $this->_DictWords($token2[TOK_VAL]);

            $match = ($token_2[TOK_VAL] == 'tengah')
            || ($this->_DictWords($token_2[TOK_VAL],DIC_CTX) == 'waktu')
            || ($record[0][DIC_CTX] == 'waktu')
            || ($record[0][DIC_FN] == 'a')
            || ($record[0][DIC_FN] == 'ad')
            || ($token_2[TOK_TYPE] == TTYP_DIGIT);

            if ($match) {
                $word = 'poé';
            } else {
                $word = $token[TOK_VAL];
            }
            $this->TermsExistHit($word);

            return true;
        }
        return false;
    }

    /* masing-masing */
    function m_masing_masing(&$token, &$token1, &$token2, &$word) {
        if ($token[TOK_VAL1] == 'masing' && $token[TOK_VAL2] == 'masing') {
            if (($record = $this->_DictWords($token2[TOK_VAL])) && $record[0][DIC_FN] == 'b') {
                $word = 'séwang-séwangna';
            } else if ($token1[TOK_TYPE] == TTYP_TERMINAL || $token1[TOK_VAL] == ',') {
                $word = 'séwang-séwangan';
            } else {
                $word = 'unggal';
            }
            $this->TermsExistHit($word);
            return true;
        }
        return false;
    }

    /* IF dictionary entry */
    function m_DataKamus(&$it, &$token, &$token2, &$word) {
        $record = $this->_DictWords($token[TOK_VAL]);
        if (isset($token2)) {
            $record2 = $this->_DictWords($token2[TOK_VAL]);

            if ($record2 && isset($record[$record2[0][DIC_FN]])) {
                $word = $record[$record2[0][DIC_FN]][DIC_TR];
                return true;
            }
        }

        if ($record) {
            if ($record[0][DIC_TR] != '') {
                $word = $record[0][DIC_TR];
            } else {
                $word = $token[TOK_VAL];
            }
            $this->TermsExistHit($word);
            return true;
        }
        return false;
    }

    function m_Sufiks(&$token, &$word) {
        if (preg_match('/(.+)(ku|mu|nya)$/i', $token[TOK_VAL],$matches)) {
            #var_dump($matches);die("".TOK_VAL."");
            if ($record = $this->_DictWords($matches[1])) {
                if ($matches[2] == 'ku') {
                    $word = $record[0][DIC_TR].' kuring';
                    $this->TermsExistHit('kuring');
                    $this->TermsExistHit($record[0][DIC_TR]);
                }
                else if ($matches[2] == 'mu') {
                    $word = $record[0][DIC_TR].' anjeun';
                    $this->TermsExistHit('anjeun');
                    $this->TermsExistHit($record[0][DIC_TR]);
                } else {
                    if ($record[0][DIC_FN] == 'v') {
                        $word = $record[0][DIC_TR]. ' manéhna';
                    } else if (preg_match('/(eun|an)$/i', $record[0][DIC_TR])) {
                        $word = $record[0][DIC_TR].'ana';
                    } else {
                        $word = $record[0][DIC_TR].'na';
                    }
                    $this->TermsExistHit($word);
                }
                return true;
            }
        }
        return false;
    }

    function m_Afiks(&$token, &$word) {
        if (preg_match('/^(se|ke|ter|di)(.+)$/i', $token[TOK_VAL],$matches)) {
            #var_dump($matches);die("".TOK_VAL."");
            if ($record = $this->_DictWords($matches[2])) {
                if ($matches[1] == 'se') {
                    $word = "sa".$record[0][DIC_TR];
                    $this->TermsExistHit($word);
                    return true;
                } else if ($matches[1] == 'ke') {
                    $word = "ka".$record[0][DIC_TR];
                    $this->TermsExistHit($word);
                    return true;
                } else if ($matches[1] == 'di') {
                    $word = "di".$record[0][DIC_TR];
                    $this->TermsExistHit($word);
                    return true;
                } else if ($record[0][DIC_FN] == 'a') {
                    $word = "pang".$record[0][DIC_TR]."na";
                    $this->TermsExistHit($word);
                    return true;
                }
            }
        }
        return false;
    }

    /* ... 'sekali' +PEUN ... */
    function m_sekali_Terminal(&$token, &$token1, &$word) {
        if ($token[TOK_VAL] == 'sekali') {
            if ($token1[TOK_TYPE] == TTYP_TERMINAL || $token1[TOK_VAL] == ',') {
                $word = 'pisan';
            } else {
                $word = 'sakali';
            }
            $this->TermsExistHit($word);
            return true;
        }
        return false;
    }

    function m_Skip(&$token, &$word)
    {
        if (($token[TOK_TYPE] == TTYP_IGNORE) || preg_match('/^((aa|x)\w+)/', $token[TOK_VAL]))
        {
            $word = $token[TOK_VAL];
            return true;
        }
        return false;
    }

    function TranslateToken($tokens, &$it, &$token, &$word) {
        $token_2 = &$tokens[$it-2];
        $token   = &$tokens[$it];
        $token2  = &$tokens[$it+2];

        $word = '';

        error_reporting(E_ERROR);
        $match
        = $this->m_Frase($it, $token, $token2, $word)
        || $this->m_NamaIstilah($token, $token_2, $word)
        || $this->m_terlalu_Ajektif($it, $token, $token2, $word)
        || $this->m_Superlatif($it, $token, $token2, $word)
        || $this->m_masih_Ajektif($it, $token, $token2, $word)
        || $this->m_kembali_Verba($it, $token, $tokens[$it+2], $word)
        || $this->m_orang($token, $token_2, $token2, $word)
        || $this->m_baru($token, $token_2, $token2, $word)
        || $this->m_dengan($token, $token2, $word)
        || $this->m_baik_atau($it, $tokens, $word)
        || $this->m_pukul_Digit($token, $token2, $word)
        || $this->m_hari($token, $token_2, $token, $word)
        || $this->m_Skip($token, $word)
        || $this->m_masing_masing($token, $tokens[$it+1], $token2, $word)
        || $this->m_sekali_Terminal($token, $tokens[$it+1],$word)
        || $this->m_DataKamus($it, $token, $token2, $word)
        || $this->m_Sufiks($token,$word)
        || $this->m_Afiks($token,$word)
        || $this->m_KataUlang($token, $word)
        ;
        error_reporting(E_ALL);

        if (!$match) {
            $word = $token[TOK_VAL];
            $this->TermsNewRegister($word);
        }
        /* Debugging:
         if (is_array($word)){
         print "match: '$match'";
         var_dump($token);
         }*/
        $uc = $token[TOK_UC];
        if ($uc == KS_UCFIRST) {
            $word = $this->GenerateUcFirst($word);
        } else if ($uc == KS_ALLCAPS) {
            $word = $this->GenerateUcAll($word);
        }
        /* ... 'buah' ... */
        /* ... ya (enya/nya) ... */
        return $match;
    }
};
?>
