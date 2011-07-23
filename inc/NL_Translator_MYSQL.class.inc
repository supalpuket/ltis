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

class NL_Translator_MYSQL extends NL_Translator_DOM {
    function NL_Translator_MYSQL() {
        parent::NL_Translator_DOM();

        if (!mysql_connect('localhost', 'tarjamah', 'tarjamah')) {
            die("Can't connect to the database");
        }
        if (!mysql_select_db('tarjamah')) {
            die("Can't select database");
        }
        mysql_query("set names utf8");

        $this->names     = array();
        $this->dictWords = array();
        $this->phrases   = array();
        $this->foreigns  = array();

        mysql_query("PREPARE namesDB   FROM 'SELECT * FROM `tarj_id_su_names`   WHERE ID = ? AND Valid > 0'");
        mysql_query("PREPARE entriesDB FROM 'SELECT * FROM `tarj_id_su_entries` WHERE ID = ? AND Valid > 0'");
        mysql_query("PREPARE mainDB    FROM 'SELECT * FROM `tarj_id_su_main`    WHERE ID = ? AND Valid > 0'");
        mysql_query("PREPARE phrasesDB FROM 'SELECT * FROM `tarj_id_su_phrases` WHERE ID = ? AND Valid > 0'");
        mysql_query("PREPARE foreignDB FROM 'SELECT * FROM `tarj_id_su_foreign` WHERE ID = ? AND Valid > 0'");
    }

    function _DictWords($key)
    {
        if (!isset($this->dictWords[$key])) {
            if ($record = $this->_DictMainWords($key)) {
                $this->dictWords[$key] = $record;
            } else {
                mysql_query("SET @key='$key'");
                $result = mysql_query("EXECUTE entriesDB USING @key");
                if (mysql_num_rows($result) == 0) {
                    $this->dictWords[$key] = false;
                } else {
                    $record = mysql_fetch_row($result);
                    if ($record[DIC_TR] == '') {
                        $record[DIC_TR] = $key;
                    }
                    $this->dictWords[$key] = $record;
                }
            }
        }
        return $this->dictWords[$key];
    }

    function _DictMainWords($key)
    {
        mysql_query("SET @key='$key'");
        $result = mysql_query("EXECUTE mainDB USING @key");
        if (mysql_num_rows($result) == 0) {
            $record = false;
        } else {
            $record = mysql_fetch_row($result);
            if ($record[DIC_TR] == '') {
                $record[DIC_TR] = $key;
            }
        }
        return $record;
    }

    function _DictPhrases($key)
    {
        if (!isset($this->phrases[$key])) {
            mysql_query("SET @key='$key'");
            $result = mysql_query("EXECUTE phrasesDB USING @key");
            if (mysql_num_rows($result) == 0) {
                $this->phrases[$key] = false;
            } else {
                $record = mysql_fetch_row($result);
                if ($record[DIC_TR] == '') {
                    $record[DIC_TR] = $key;
                }
                $this->phrases[$key] = $record;
            }
        }
        return $this->phrases[$key];
    }

    function _DictNames($key)
    {
        if (!isset($this->names[$key])) {
            mysql_query("SET @key='$key'");
            $result = mysql_query("EXECUTE namesDB USING @key");
            if (mysql_num_rows($result) == 0) {
                $this->names[$key] = false;
            } else {
                $record = mysql_fetch_row($result);
                if ($record[DIC_TR] == '') {
                    $record[DIC_TR] = $key;
                }
                $this->names[$key] = $record;
            }
        }
        return $this->names[$key];
    }

    function _DictForeigns($key)
    {
        if (!isset($this->foreigns[$key])) {
            mysql_query("SET @key='$key'");
            $result = mysql_query("EXECUTE foreignDB USING @key");
            if (mysql_num_rows($result) == 0) {
                $this->foreigns[$key] = false;
            } else {
                $record = mysql_fetch_row($result);
                if ($record[DIC_TR] == '') {
                    $record[DIC_TR] = $key;
                }
                $this->foreigns[$key] = $record;
            }
        }
        return $this->foreigns[$key];
    }
}
?>