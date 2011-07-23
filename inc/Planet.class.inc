<?php

define('MAGPIE_DIR', 'inc/');
define('MAGPIE_CACHE_ON', true);
if (PLANET_ITEMS == 'PLANET_ITEMS') {
    define('PLANET_ITEMS',20);
}
require_once(MAGPIE_DIR.'rss_fetch.inc');
require_once(TARJ_DIR.'TarjamahID.class.inc');

function planet_cmp($a, $b)
{
    if ($a['issued'] == $b['issued']) {
        return 0;
    }
    return ($a['issued'] > $b['issued']) ? -1 : 1;
}

class Planet {
    function Planet() {
        $lines = file('planet.urls');
        $this->feedUrls = array();
        foreach ($lines as $line) {
            list($url,$description) = explode(';',$line);
            $url = trim($url);
            if ($url != '') $this->feedUrls[] = $url;
        }
        $this->feeders = array();
        $this->tarjamah = new TarjamahID();
        $this->tarjamah->InitURLBase('');
    }

    function Destroy() {
        $this->tarjamah->TermsNewSave();
        unset($this->tarjamah);
        unset($this->feedUrls);
        unset($this->rssItems);
    }

    function SanitizeDateTime(&$item) {
        #var_dump($item);die();
        if ($item['dc']['creator']) {
            $item['author_name'] = $item['dc']['creator'];
        }
        if ($item['date_timestamp']) {
            $item['issued'] = date('c',$item['date_timestamp']);
        }
        list($date,$time) = explode("T",$item['issued']);
        $item['date'] = date("F j, Y",strtotime($date));
        $item['time'] = $time;
    }

    function SanitizeItemContent(&$item) {
        $content = '';
        if ($item['atom_content']) {
            $content = $item['atom_content'];
        } else if ($item['summary']) {
            $content = $item['summary'];
        } else {
            $content = $item['description'];
        }
        $item['content'] = $this->tarjamah->TranslateHTML($content);
        $item['title'] = $this->tarjamah->TranslateText($item['title']);
        $this->tarjamah->SanitizeLink($item['link']);
    }

    function FetchRssItems() {
        $feedUrls = &$this->feedUrls;
        $items = array();
        foreach ($feedUrls as $url) {
            $rss = fetch_rss($url);
            $nItems = count($rss->items);
            $channelLink = "<a href=\"".$rss->channel['link']."\">".$rss->channel['title']."</a>";
            for($it=0;$it<$nItems;$it++) {
                $rss->items[$it]['channel'] = $channelLink;
                $this->SanitizeDateTime($rss->items[$it]);
            }
            $this->feeders[] = $channelLink;
            $items = &array_merge($items,$rss->items);
        }
        usort($items,"planet_cmp");
        reset($items);

        $rssItems = array();
        $it = 0;
        foreach ($items as $item) {
            $this->SanitizeItemContent($item);
            $rssItems[] = $item;
            if ($it<PLANET_ITEMS) {
                $it++;
            } else {
                break;
            }
        }
        return $this->rssItems = $rssItems;
    }

    function PrintFeederList() {
        $output = '';
        foreach ($this->feeders as $feeder) {
            $output .= "<li>$feeder</li>\n";
        }
        $output = "<ul>\n$output</ul>\n";
        print $output;
    }

    function ListOfContents(&$rssItems) {
        $output = '';
        $it = 0;
        foreach ($rssItems as $item) {
            $output .= "<li>$item[date] &rArr; <a href=\"#Content-$it\">$item[title]</a> by $item[author_name]</li>";
            $it++;
        }
        return "<ul>\n".$output."</ul>\n";
    }

    function FormatItem(&$item, $it) {
        #var_dump($item);die();

        #".str_replace('<br ></br>','<br/>',@implode(" ",$item['content'])).";
        $output = "<div class=\"post\">\n";
        $output .= "<a name=\"Content-$it\"></a>";
        $output .= "<h3 class=\"post-title\"><a href=$item[link]>$item[title]</a></h3>\n";
        $output .= "<div class=\"post-body\">\n$item[content]</div>\n";
        $output .= "<p class=\"post-footer\">posted by $item[author_name] @ $item[channel], $item[date] $item[time]</p>\n";
        $output .= "</div>\n";

        return $output;
    }

    function AllContents(&$rssItems) {
        $output = '';
        $it = 0;
        $cdate = '';
        foreach ($rssItems as $item) {
            #var_dump($item);die();
            if($cdate != $item['date']) {
                $output .= "<h2 class=\"date-header\">$item[date]</h2>\n";
                $cdate = $item['date'];
            }
            $output .= $this->FormatItem($item,$it);
            $it++;
        }
        return $output;
    }

    function PrintAllContents() {
        $rssItems = $this->FetchRssItems();

        //echo "<div class=\"post\">";
        echo "<h3 class=\"post-title\"><a href=\"".$_SERVER['PHP_SELF']."\">".PLANET_TOC_TITLE."</a></h3>\n";
        echo "<div class=\"post-body\">".PLANET_NOTICE."</div>";
        echo $this->ListOfContents($rssItems);
        echo $this->AllContents($rssItems);
        //echo "</div>";
    }
} //

?>
