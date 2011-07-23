<?php
/**
 * $Id: teks.php 3 2009-02-22 22:57:27Z NaDiN $
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

define ('TARJ_DIR','inc/');

require_once(TARJ_DIR.'NL_Translator_ID2SU.class.inc');

$tarjamah = new NL_Translator_ID2SU();
$tarjamah->HandleRequestText();
?>