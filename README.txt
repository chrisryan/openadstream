$Id: README.txt,v 1.1.2.2 2009/12/30 05:11:11 damienmckenna Exp $

Open AdStream Module Instructions
*********************************

The Open AdStream (OAS) module allows Drupal sites to display advertisements
from service of the same name by the company 24/7 Real Media.  Advertisements
are controlled using blocks which load a given ad, identified using an OAS-
supplied "position" name, in a designated spot on the page using standard
Drupal practices, e.g. using the standard block visibility rules or by adding
it to a page created using the Panels module.

Note: this system only controls which ad positions are displayed and on which
pages they are displayed, it does not control which specific advertisements
are displayed within those ad positions - that is still controlled within the
normal 24/7 Real Media OAS control panel.

There are four parts to configuring Open AdStream:
 1. General settings.
 2. Gutter ads.
 3. Pagenames.
 4. Position names.

During the process of obtaining advertisements from the OAS system, a request
is submitted in the approximate format:
* http://[oas_hostname]/[site_hostname]/[pagename]/[positions]
The first two parts are controlled in the General Settings page (#1 below),
the third is in the Pagename settings (#3 below) and the final the position
names (#4 below).


1. General Settings - admin/build/openadstream
**********************************************

The two most important settings are the Open AdStream Hostname and the Site
Hostname.  The former is the hostname used to load advertisements, will be
provided by 24/7 Real Media and will be either a generic hostname like
"oascentral.247realmedia.com" which is a subdomain of 24/7 Real Media's own
domain, or something customized for the account, e.g. for the domain
"example.com" it might be "oascentral.example.com".  The latter setting is
the hostname as configured within the OAS system and will almost always be
the full hostname without the leading "www.", e.g. for the domain
"www.example.com" it would be most likely be "example.com"; one exception
to this rule is a separate hostname that may be provided for development
purposes, to help ensure the ads are loaded correctly before the account is
billed for page impressions.

There are two optional settings:
* for certain scenarios it may be necessary to assign the same pagename value
  (see below) across all pages; in these cases assign the value to the "Fixed
  Page Name" field.
* If the site is being run from a subdirectory under the domain, e.g. 
  "http://example.com/testsite/", by default it will not include the
  subdirectory name in the pagename value (see below); if the opposite is
  required, e.g. the pagename should include "testsite/" at the beginning,
  then enable this setting.

2. Gutter Ads - admin/build/openadstream/gutters
************************************************

There is no clear way to display advertisements in the background of a page,
due to a combination of factors, including limitations in Flash (commonly used
in advertisements), page object overlapping, etc.  Rather than attempting to
show an advertisement around the page content, a system is provided to
optionally show advertisements to the left and right of the page's content:
put simply this allows the page's content to be wrapped by ads akin to the
bread in a sandwich.

Gutter ads are controlled using several settings which allow for a great deal
of flexibility for different scenarios.  Four main settings are used to
control basic values that are applied to all advertisements:

* Left Gutter Position: this is an ad position name which is used to display
  the ad shown in the left page gutter: the space between the outer left side
  of the page content and the browser window.
* Right Gutter Position: this is an ad position name which is used to display
  the ad shown in the right page gutter: the space between the outer right
  side of the page content and the browser window.
* DOM Element: this is the outer-most page element from the output HTML which
  will be used to decide how wide to make the gutters.  This will usually be
  the first DIV within the site's theme's page.tpl.php file and is commonly
  named 'page'.  This is controlled using jQuery selection strings, so if the
  element has an 'id' value of "page" it should be preceded by a hash symbol,
  i.e. "#page", likewise to use an element whose class attribute is "content"
  use the value ".content".
* Padding: by default the gutters will be flush against the page content;
  depending on the site layout it may be preferable to have some padding
  between the content and the gutter, which is controlled using this field.

Along with the basic settings, gutter ads must be enabled for specific paths.
By default gutters are enabled for all pages except for admin, node/add and
node/edit pages, and this may be modified to include additional paths or,
more likely, remove paths from the list.

Gutters are controlled by first loading the paths that are set to be
*included* and then seeing if it should be *excluded*.  As an example, if the
path "products/*" is set to be included and "products/elephants" to be
excluded, all paths that start with "products/" will display ads with the
exception of the products/elephants page.  Each path setting is controlled
individually by creating a new "Gutter Ad" setting, deciding if the given
path is to be included or excluded and assigning a specific list of paths
that should match; in the interest of flexibility, the "*" symbol may be used
as a wildcard, similar to block visibility rules.  Additional settings allow
the path to only be active within a specific date range, e.g. to coincide
with a specific marketing campaign, and an optional background color that is
applied to the whole page.

3. Pagenames - admin/build/openadstream/pagenames
*************************************************

The pagename value submitted to OAS is for uniquely identifying a given URL
so that it may be correctly identified thus control the advertisements
displayed to visitors on that page.  There are several ways which pagename
values can be assigned:
* By default the page's path will be assigned as the pagename, e.g. the page
  "http://example.com/products/elephants" will use "products/elephants" as
  the pagename.
* Paths can have pagename values assigned using specific paths via the
  settings page above.  Similarly to gutter ads, these paths may use
  wildcards to match multiple paths more easily, e.g. the setting
  "products/*" will match all paths that start with "products/" (note: it
  does not, however, match "products", for that add another line or change
  the setting to "products*").
* Individual nodes can have a specific pagename value assigned on its
  node-edit page by those who have the correct permission.

4. Position Names - admin/build/openadstream/positions
******************************************************

This module does not control the specific advertisements loaded on the page,
instead it only controls the OAS ad positions available on the page, the ads
that are shown are controlled using the normal 24/7 Real Media control panel.
Ad position names are standardized within the OAS system with many commonly
used ones, including "Top", "Left", "Right", etc.  Each position name may be
provided an optional description which can help identify its purpose, e.g.
it might be helpful to describe the "Top" position name as "Leaderboard ad
used in the page header".

A fundamental design point of this module is that only the position names
actually being loaded on a given page will actually be loaded through the
OAS code, thus the account will only be billed for ads that are actually
displayed on each page, not all of them.  As a result it is safe to list all
of the positions that may be used on the site rather than only limiting it
to those currently in use.


Developer Notes
***************

Much of the Open AdStream module may be configured through various hooks,
please see the separate API.txt file for further details.


Contributors - see CHANGELOG.txt
********************************

Originally written by Damien McKenna (http://drupal.org/user/108450) in 2008
for the Bonnier Corporation's Drupal 5 site skinet.com; upgraded to work with
Drupal 6 during 2009 for scubadiving.com.  Major rewrite in 2008 by Lewis
Vance (http://drupal.org/user/340383) for the upgraded skinet.com site.
CTools integration added by Dale "datacaliber" Jung
(http://drupal.org/user/469130).
