/* *********************************************************************
 * This Original Work is copyright of 51 Degrees Mobile Experts Limited.
 * Copyright 2019 51 Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY.
 *
 * This Original Work is licensed under the European Union Public Licence (EUPL)
 * v.1.2 and is subject to its terms as set out below.
 *
 * If a copy of the EUPL was not distributed with this file, You can obtain
 * one at https://opensource.org/licenses/EUPL-1.2.
 *
 * The 'Compatible Licences' set out in the Appendix to the EUPL (as may be
 * amended by the European Commission) shall be deemed incompatible for
 * the purposes of the Work and the provisions of the compatibility
 * clause in Article 5 of the EUPL shall not apply.
 *
 * If using the Work as, or as part of, a network application, by
 * including the attribution notice(s) required under Article 5 of the EUPL
 * in the end user terms of the application under an appropriate heading,
 * such notice(s) shall fulfill the requirements of that article.
 * ********************************************************************* */

const {Builder, By} = require('selenium-webdriver');
const chrome = require('selenium-webdriver/chrome');
const firefox = require('selenium-webdriver/firefox');

// Varnish needs to be running with the JavaScript example, and the page served
// over HTTP, as a page opened from a file cannot keep the cookie the script sets.
const page = process.env.PAGE_URL || 'http://localhost:8889/index.html';
const timeoutMilliseconds = 20000;

async function checkWidth(driver) {
	try {
		await driver.get(page);
		const width = await driver.wait(async () => {
			const found = await driver.findElements(By.id('widthvalue'));
			return found.length > 0 ? found[0].getText() : false;
		}, timeoutMilliseconds);
		// A real width, which only the browser knows, rather than Unknown.
		expect(width).toMatch(/^\d+$/);
	}
	finally {
		await driver.quit();
	}
}

describe('Javascript tests', () => {
	test('Javascript in Chrome', async () => {
		// The HeadlessChrome User-Agent is not one a person would send, so it does not
		// get the JavaScript this test is for.
		const options = new chrome.Options().addArguments(
			'--headless=new',
			'--no-sandbox',
			'--user-agent=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36');
		const driver = await new Builder()
			.forBrowser('chrome')
			.setChromeOptions(options)
			.build();
		await checkWidth(driver);
	}, timeoutMilliseconds * 2);

	test('Javascript in Firefox', async () => {
		const options = new firefox.Options().addArguments('-headless');
		const driver = await new Builder()
			.forBrowser('firefox')
			.setFirefoxOptions(options)
			.build();
		await checkWidth(driver);
	}, timeoutMilliseconds * 2);
});
