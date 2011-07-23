<?php
class NL_UrlSanitizer
{
	function SanitizeLink(&$link, $usebase = true, $css = false, $debug = false) {
		#var_dump($this->urlbase);
		if (preg_match('/^mailto/i', $link)) {
			$usebase = false;
		}
			
		$url = @parse_url($link);
		if (!$url) return;
		if (!isset($url['scheme'])) {
			if (isset($url['path']) && $url['path'][0] == '/') {
				$link = $this->urlbase['host']."$link";
				if ($debug) print "$link<br/>\n";
			} else if (isset($this->urlbase['host'])) {
				if (!isset($this->urlbase['path'])) {
					$path = '';
				} else {
					$path = $this->urlbase['path'];
				}
				$link = $this->urlbase['host'].$path."/$link";
			} else {
				$link = $this->urlbase['path']."/$link";
			}
		}
		if ($css) {
			$link = $this->urlbase['css']."?q=$link";
		} else if (!preg_match('/\.(jpg|gif|pdf|doc|xls|png|tif|wav|mp3|mpg|mp4|aac|ogg|swf|jar)/i', $link) && $usebase) {
			$link = $this->urlbase['self']."?article=$link";
		}
	}

	function InitURLBase($inputURL, $self = false) {
		$this->urlbase['input'] = $inputURL;
		if ($self) {
			$this->urlbase['self'] = $self;
		} else {
			$this->urlbase['self'] = 'loka.php';
		}
		$this->urlbase['css'] = 'css.php';

		$urlcomponent = parse_url($inputURL);
		if (isset($urlcomponent['scheme'])) {
			if ($urlcomponent['scheme'] == 'http' || $urlcomponent['scheme'] == 'https') {
				$this->urlbase['host'] = "$urlcomponent[scheme]://$urlcomponent[host]";
			} else if ($urlcomponent['scheme'] == 'file' && isset($urlcomponent['host'])) {
				$this->urlbase['host'] = "file:///$urlcomponent[host]";
			} else if (isset($urlcomponent['host'])) {
				$this->urlbase['host'] = "$urlcomponent[scheme]:$urlcomponent[host]";
			} else {
				$this->urlbase['host'] = $urlcomponent['scheme'].':';
			}
		}
		else if (isset($urlcomponent['host'])) {
			$this->urlbase['host'] = $urlcomponent['host'];
		}

		if (isset($urlcomponent['path']))
		{
			if ($urlcomponent['path'][($len = strlen($urlcomponent['path'])-1)] == '/') {
				$this->urlbase['path'] = substr($urlcomponent['path'], 0, $len);
			} else {
				$pos = strrpos($urlcomponent['path'], '/');
				$this->urlbase['path'] = substr($urlcomponent['path'], 0, $pos);
			}
		}

		if (isset($urlcomponent['query'])) {
			$this->urlbase['query'] =  $urlcomponent['query'];
		}

		#print "---------------\nurlbase = "; var_dump($this->urlbase);
	}

	function TestURLs($url)
	{
		foreach ($url as $link) {
			$olink = $link;
			$this->SanitizeLink($link, false);
			print "<tr><td>$olink</td><td>$link</td></tr>\n";
		}
	}

	function TestURLBase() {
		$url = array(
            'include/styles.css',
                '/koran.asp',
                'arsip.asp',
                'http://google.com' );
			
		print '<table>';
		$this->InitURLBase('file:///d:/WebProjekte/tarjamah/test/Ratusan_Kilometer_Jalan_di_Indramayu_Rusak_Akibat_Banjir.htm');
		$this->TestURLs($url);
		$this->InitURLBase('file:///d:/WebProjekte/tarjamah/test/');
		$this->TestURLs($url);
		$this->InitURLBase('d:/WebProjekte/tarjamah/test/Ratusan_Kilometer_Jalan_di_Indramayu_Rusak_Akibat_Banjir.htm');
		$this->TestURLs($url);
		$this->InitURLBase('d:/WebProjekte/tarjamah/test/');
		$this->TestURLs($url);
		$this->InitURLBase('http://republika.co.id/koran.asp');
		$this->TestURLs($url);
		$this->InitURLBase('http://republika.co.id/hello/koran.asp');
		$this->TestURLs($url);
		$this->InitURLBase('http://republika.co.id');
		$this->TestURLs($url);
		$this->InitURLBase('http://republika.co.id/');
		$this->TestURLs($url);
		$this->InitURLBase('http://republika.co.id/index/');
		$this->TestURLs($url);
		print '</table>';
	}

}
?>