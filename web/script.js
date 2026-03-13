const input = document.getElementById("query")
const button = document.getElementById("searchBtn")

const results = document.getElementById("results")
const stats = document.getElementById("stats")

const docView = document.getElementById("docView")

const docName = document.getElementById("docName")
const docPath = document.getElementById("docPath")

const docText = document.getElementById("docText")

const prevBtn = document.getElementById("prevMark")
const nextBtn = document.getElementById("nextMark")
const backBtn = document.getElementById("backBtn")

const template = document.getElementById("result-template")

const markIndex = document.getElementById("markIndex")
const markMap = document.getElementById("markMap")

const docCache = new Map()

let marks = []
let current = 0
let currentDocId = null
let prevDocMark = null

let snippetMarks = []
let snippetIndex = 0
let prevSnippet = null

let navTimer = null
let lastSearchData = null
let inDocument = false

/* QUERY CACHE */

let queryWords = []
let queryRegex = null

const form = document.getElementById("searchForm")

form.addEventListener("submit", e => {

    e.preventDefault()

    search()

    input.focus()

})

/* BACK */

backBtn.onclick = () => {

    inDocument = false

    docView.style.display = "none"
    results.style.display = "block"
    stats.style.display = "block"

    prevBtn.style.display = "none"
    nextBtn.style.display = "none"

    input.focus()
}

/* SEARCH */

async function search() {

    stopNav()

    const q = input.value.trim()
    if (!q) return

    if (inDocument) {

        backBtn.onclick()

    }

    queryWords = q.split(/\s+/).filter(Boolean)

    queryRegex = new RegExp(
        "(" + queryWords.map(escapeRegex).join("|") + ")",
        "gi"
    )

    const start = performance.now()

    const resp = await fetch("/search?q=" + encodeURIComponent(q))
    const data = await resp.json()

    lastSearchData = data

    render(data, start)
}

/* RENDER */

function formatScore(s) {

    const num = Number(s)

    if (!Number.isFinite(num))
        return "0.0"

    let v = Math.round(num * 1000) / 1000
    let str = v.toString()

    if (!str.includes(".")) str += ".0"

    return str
}

function resetNav() {

    snippetMarks = []
    snippetIndex = 0
    prevSnippet = null

    marks = []
    current = 0
    prevDocMark = null

    prevBtn.style.display = "none"
    nextBtn.style.display = "none"
}

function render(data, start) {

    resetNav()

    results.innerHTML = ""
    markMap.innerHTML = ""

    const frag = document.createDocumentFragment()

    for (const result of data.results) {

        const card = template.content.cloneNode(true)

        const link = card.querySelector(".result-link")
        const path = card.querySelector(".result-path")
        const score = card.querySelector(".result-score")
        const snippet = card.querySelector(".result-snippet")

        link.textContent = getFileName(result.path)
        link.href = "#"

        path.textContent = result.path

        score.textContent =
            "score: " + (result.score != null ? formatScore(result.score) : "0.0")

        snippet.innerHTML = ""

        link.onclick = async e => {

            e.preventDefault()

            if (!snippet.dataset.loaded) {

                let text

                if (docCache.has(result.docid)) {
                    text = docCache.get(result.docid)
                } else {
                    const rdoc = await fetch(`/doc?id=${result.docid}`)
                    text = await rdoc.text()
                    docCache.set(result.docid, text)
                }

                const snips = buildSnippets(text)

                snips.forEach(s => {

                    const div = document.createElement("div")
                    div.className = "snippet-block"
                    div.innerHTML = highlight(s)

                    snippet.appendChild(div)

                })

                snippet.dataset.loaded = "1"
            }

            document.querySelectorAll(".result-snippet.show")
                .forEach(s => {
                    if (s !== snippet)
                        s.classList.remove("show")
                })

            snippet.classList.toggle("show")

            if (snippet.classList.contains("show")) {

                snippetMarks = [...snippet.getElementsByTagName("mark")]
                snippetIndex = 0
                prevSnippet = null

                highlightCurrentSnippet()

                prevBtn.style.display = "inline-block"
                nextBtn.style.display = "inline-block"

            } else {

                prevBtn.style.display = "none"
                nextBtn.style.display = "none"

            }
        }

        path.onclick = () => openDoc(result.docid, result.path)

        frag.appendChild(card)
    }

    results.appendChild(frag)

    stats.textContent =
        data.results.length + " results (" +
        (performance.now() - start).toFixed(1) + " ms)"
}

/* SNIPPET NAVIGATION */

function highlightCurrentSnippet() {

    if (!snippetMarks.length) return

    if (prevSnippet)
        prevSnippet.style.background = "yellow"

    const m = snippetMarks[snippetIndex]

    m.style.background = "orange"

    prevSnippet = m

    m.scrollIntoView({ block: "center" })
}

function highlightCurrentDoc() {

    if (!marks.length) return

    if (prevDocMark)
        prevDocMark.style.background = "yellow"

    const m = marks[current]

    m.style.background = "orange"

    prevDocMark = m

    docText.scrollTop =
        m.offsetTop - docText.clientHeight / 2

    updateCounter()
}

function goSnippet(step) {

    if (!snippetMarks.length) return

    snippetIndex += step

    if (snippetIndex < 0) snippetIndex = snippetMarks.length - 1
    if (snippetIndex >= snippetMarks.length) snippetIndex = 0

    highlightCurrentSnippet()
}

function startNav(step) {

    stopNav()

    navTimer = setInterval(() => {

        if (inDocument)
            go(step)
        else
            goSnippet(step)

    }, 120)

}

function stopNav() {

    if (navTimer) {
        clearInterval(navTimer)
        navTimer = null
    }

}

prevBtn.onmousedown = () => startNav(-1)
nextBtn.onmousedown = () => startNav(1)

prevBtn.onclick = () => {
    if (docView.style.display === "block")
        go(-1)
    else
        goSnippet(-1)
}

nextBtn.onclick = () => {
    if (docView.style.display === "block")
        go(1)
    else
        goSnippet(1)
}

document.addEventListener("mouseup", stopNav)
prevBtn.onmouseleave = stopNav
nextBtn.onmouseleave = stopNav

/* OPEN DOC */

async function openDoc(id, path) {

    results.style.display = "none"
    stats.style.display = "none"
    docView.style.display = "block"

    markMap.innerHTML = ""
    marks = []
    current = 0
    prevDocMark = null

    inDocument = true

    let text

    if (docCache.has(id)) {
        text = docCache.get(id)
    } else {
        const r = await fetch("/doc?id=" + id)
        text = await r.text()
        docCache.set(id, text)
    }

    currentDocId = id
    docName.textContent = getFileName(path)
    docPath.textContent = path

    docText.innerHTML = highlight(text)

    collectMarks()

    if (marks.length) {

        current = 0
        prevDocMark = null

        highlightCurrentDoc()

        updateCounter()

        prevBtn.style.display = "inline-block"
        nextBtn.style.display = "inline-block"

    } else {

        prevBtn.style.display = "none"
        nextBtn.style.display = "none"
    }
}

/* MULTI SNIPPETS */

function buildSnippets(text, radius = 40, limit = 5) {

    const snippets = []
    let pos = 0
    const lower = text.toLowerCase()

    while (snippets.length < limit) {

        let found = -1
        let wordLen = 0

        const wordsLower = queryWords.map(w => w.toLowerCase())

        for (const w of wordsLower) {

            const i = lower.indexOf(w.toLowerCase(), pos)

            if (i !== -1 && (found === -1 || i < found)) {
                found = i
                wordLen = w.length
            }
        }

        if (found === -1) break

        let start = Math.max(0, found - radius)
        let end = Math.min(text.length, found + wordLen + radius)

        while (start > 0 && /\w/.test(text[start])) start--
        while (end < text.length && /\w/.test(text[end])) end++

        let snip = text.slice(start, end)

        if (start > 0) snip = "… " + snip
        if (end < text.length) snip += " …"

        snippets.push(snip)

        pos = found + wordLen
    }

    return snippets
}

/* HIGHLIGHT */

function highlight(text) {

    if (!queryRegex) return text

    let result = text

    const wordsLower = queryWords.map(w => w.toLowerCase())

    for (const w of wordsLower) {

        const re = new RegExp("(" + escapeRegex(w) + ")", "gi")

        result = result.replace(re, "<mark>$1</mark>")
    }

    return result
}

/* DOC MARKS */

function collectMarks() {

    marks = Array.from(docText.getElementsByTagName("mark"))

    buildMap()
    updateCounter()
}

function go(step) {

    if (!marks.length) return

    current += step

    if (current < 0) current = marks.length - 1
    if (current >= marks.length) current = 0

    highlightCurrentDoc()
}

function updateCounter() {

    markIndex.textContent =
        (current + 1) + " / " + marks.length
}

/* MAP */

function buildMap() {

    markMap.innerHTML = ""

    const total = docText.scrollHeight

    marks.forEach((m, i) => {

        const dot = document.createElement("div")

        dot.className = "markDot"

        dot.style.top = (m.offsetTop / total * 100) + "%"

        dot.onclick = () => {
            current = i
            go(0)
        }

        markMap.appendChild(dot)
    })
}

/* UTILS */

function escapeRegex(t) {
    return t.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")
}

function getFileName(path) {
    const p = path.split(/[\/\\]/)
    return p[p.length - 1]
}