"use strict";

const API_PREFIX = "/api";
const MAX_CARDS = 24;
const MAX_SONGS = 40;

const dom = {
  form: document.querySelector("#searchForm"),
  input: document.querySelector("#searchInput"),
  clear: document.querySelector("#clearSearch"),
  hero: document.querySelector("#hero"),
  results: document.querySelector("#results"),
  empty: document.querySelector("#emptyState"),
  status: document.querySelector("#statusMessage"),
  title: document.querySelector("#resultsTitle"),
  total: document.querySelector("#resultTotal"),
  artistsSection: document.querySelector("#artistsSection"),
  albumsSection: document.querySelector("#albumsSection"),
  songsSection: document.querySelector("#songsSection"),
  artistsRow: document.querySelector("#artistsRow"),
  albumsRow: document.querySelector("#albumsRow"),
  songsList: document.querySelector("#songsList"),
  artistsCount: document.querySelector("#artistsCount"),
  albumsCount: document.querySelector("#albumsCount"),
  songsCount: document.querySelector("#songsCount"),
  apiStatus: document.querySelector("#apiStatus"),
  apiCard: document.querySelector(".sidebar-card")
};

const palettes = [
  ["#f42b35", "#710006"],
  ["#d50000", "#210000"],
  ["#ff4b52", "#4f0004"],
  ["#a90009", "#160102"],
  ["#ef1722", "#370004"],
  ["#bd0811", "#0d0d0f"]
];

let searchSequence = 0;

function createElement(tag, className, text) {
  const element = document.createElement(tag);
  if (className) element.className = className;
  if (text !== undefined) element.textContent = text;
  return element;
}

function icon(type) {
  const icons = {
    album: '<svg viewBox="0 0 24 24" aria-hidden="true"><circle cx="12" cy="12" r="9"/><circle cx="12" cy="12" r="2.5" fill="var(--cover-b)"/></svg>',
    external: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M14 5h5v5M10 14 19 5M19 13v6H5V5h6"/></svg>'
  };
  return icons[type] || "";
}

function normalizeMusic(raw) {
  return {
    name: String(raw?.Name ?? raw?.name ?? "Música sem nome").trim(),
    singer: String(raw?.Singer ?? raw?.singer ?? "Artista desconhecido").trim(),
    album: String(raw?.Album ?? raw?.album ?? raw?.album_name ?? "Álbum desconhecido").trim(),
    genre: String(raw?.Genre ?? raw?.genre ?? "Gênero não informado").trim(),
    url: String(raw?.URL ?? raw?.url ?? "").trim(),
    popularity: Number(raw?.Popularity ?? raw?.popularity ?? 0),
    duration: Number(raw?.Duration ?? raw?.duration ?? raw?.duration_ms ?? 0)
  };
}

function uniqueBy(items, keyFn) {
  const map = new Map();
  for (const item of items) {
    const key = keyFn(item).toLocaleLowerCase("pt-BR");
    if (!map.has(key)) map.set(key, item);
  }
  return [...map.values()];
}

function hashText(value) {
  let hash = 0;
  for (let i = 0; i < value.length; i += 1) hash = ((hash << 5) - hash + value.charCodeAt(i)) | 0;
  return Math.abs(hash);
}

function applyPalette(element, value) {
  const [a, b] = palettes[hashText(value) % palettes.length];
  element.style.setProperty("--cover-a", a);
  element.style.setProperty("--cover-b", b);
}

function initials(value) {
  const words = value.split(/\s+/).filter(Boolean);
  return words.slice(0, 2).map((word) => word[0]).join("").toUpperCase() || "♪";
}

function formatDuration(ms) {
  if (!Number.isFinite(ms) || ms <= 0) return "--:--";
  const totalSeconds = Math.round(ms / 1000);
  const minutes = Math.floor(totalSeconds / 60);
  const seconds = String(totalSeconds % 60).padStart(2, "0");
  return `${minutes}:${seconds}`;
}

function plural(count, singular, pluralForm) {
  return `${count} ${count === 1 ? singular : pluralForm}`;
}

function validHttpUrl(value) {
  try {
    const parsed = new URL(value);
    return ["http:", "https:"].includes(parsed.protocol) ? parsed.href : "";
  } catch {
    return "";
  }
}

async function apiGet(route) {
  const response = await fetch(`${API_PREFIX}${route}`, {
    headers: { Accept: "application/json" },
    signal: AbortSignal.timeout(20000)
  });

  const contentType = response.headers.get("content-type") || "";
  const payload = contentType.includes("application/json") ? await response.json() : null;
  if (!response.ok) throw new Error(payload?.Erro || `A API respondeu com status ${response.status}.`);
  return Array.isArray(payload) ? payload.map(normalizeMusic) : [];
}

function setStatus(message = "", type = "") {
  dom.status.textContent = message;
  dom.status.className = `status-message${type ? ` ${type}` : ""}`;
}

function setApiStatus(connected, message) {
  dom.apiStatus.textContent = message;
  dom.apiCard.classList.toggle("connected", connected);
  dom.apiCard.classList.toggle("disconnected", !connected);
}

function showLoading(query) {
  dom.hero.hidden = true;
  dom.empty.hidden = true;
  dom.results.hidden = false;
  dom.title.textContent = `Buscando “${query}”`;
  dom.total.textContent = "";
  setStatus("Consultando músicas, artistas e álbuns…");

  [dom.artistsSection, dom.albumsSection].forEach((section) => { section.hidden = false; });
  dom.songsSection.hidden = true;
  [dom.artistsRow, dom.albumsRow].forEach((row) => {
    row.replaceChildren();
    row.classList.add("loading-grid");
    for (let i = 0; i < 5; i += 1) {
      row.append(document.querySelector("#skeletonTemplate").content.cloneNode(true));
    }
  });
}

function createArtistCard(artist, songs) {
  const card = createElement("article", "media-card artist-card");
  card.tabIndex = 0;
  card.setAttribute("role", "button");
  card.setAttribute("aria-label", `Pesquisar pelo artista ${artist}`);

  const cover = createElement("div", "media-cover");
  applyPalette(cover, artist);
  cover.append(createElement("span", "artist-initials", initials(artist)));

  const artistSongs = songs.filter((song) => song.singer.toLocaleLowerCase("pt-BR") === artist.toLocaleLowerCase("pt-BR"));
  const genres = uniqueBy(artistSongs, (song) => song.genre).map((song) => song.genre).slice(0, 2).join(" • ");
  card.append(cover, createElement("h4", "", artist), createElement("p", "", genres || "Artista"));
  card.append(createElement("div", "meta", plural(artistSongs.length, "música encontrada", "músicas encontradas")));

  const action = () => runSearch(artist);
  card.addEventListener("click", action);
  card.addEventListener("keydown", (event) => {
    if (event.key === "Enter" || event.key === " ") { event.preventDefault(); action(); }
  });
  return card;
}

function createAlbumCard(album, songs) {
  const albumSongs = songs.filter((song) => song.album.toLocaleLowerCase("pt-BR") === album.toLocaleLowerCase("pt-BR"));
  const representative = albumSongs[0] || {};
  const card = createElement("article", "media-card");
  card.tabIndex = 0;
  card.setAttribute("role", "button");
  card.setAttribute("aria-label", `Pesquisar pelo álbum ${album}`);

  const cover = createElement("div", "media-cover");
  applyPalette(cover, album);
  cover.innerHTML = icon("album");
  card.append(cover, createElement("h4", "", album), createElement("p", "", representative.singer || "Artista desconhecido"));
  card.append(createElement("div", "meta", representative.genre || "Álbum"));

  const action = () => runSearch(album);
  card.addEventListener("click", action);
  card.addEventListener("keydown", (event) => {
    if (event.key === "Enter" || event.key === " ") { event.preventDefault(); action(); }
  });
  return card;
}

function createSongItem(song, index) {
  const item = createElement("article", "song-item");
  applyPalette(item, `${song.name}${song.singer}`);

  const number = createElement("div", "song-number", String(index + 1).padStart(2, "0"));
  const main = createElement("div", "song-main");
  main.append(createElement("strong", "", song.name), createElement("span", "", song.singer));

  const album = createElement("div", "song-data");
  album.append(createElement("small", "", "Álbum"), createElement("span", "", song.album));

  const genre = createElement("div", "song-data");
  genre.append(createElement("small", "", "Gênero"), createElement("span", "", song.genre));

  const duration = createElement("div", "song-duration", formatDuration(song.duration));
  const url = validHttpUrl(song.url);
  const link = createElement("a", `song-link${url ? "" : " disabled"}`);
  link.innerHTML = icon("external");
  link.href = url || "#";
  link.target = "_blank";
  link.rel = "noopener noreferrer";
  link.setAttribute("aria-label", url ? `Abrir link de ${song.name}` : "Link indisponível");

  item.append(number, main, album, genre, duration, link);
  return item;
}

function renderResults(query, artistsSource, albumsSource, songs) {
  const artists = uniqueBy(artistsSource, (item) => item.singer).map((item) => item.singer);
  const albums = uniqueBy(albumsSource, (item) => `${item.album}|${item.singer}`).map((item) => item.album);

  dom.artistsRow.classList.remove("loading-grid");
  dom.albumsRow.classList.remove("loading-grid");
  dom.artistsRow.replaceChildren(...artists.slice(0, MAX_CARDS).map((artist) => createArtistCard(artist, artistsSource)));
  dom.albumsRow.replaceChildren(...albums.slice(0, MAX_CARDS).map((album) => createAlbumCard(album, albumsSource)));
  dom.songsList.replaceChildren(...songs.slice(0, MAX_SONGS).map(createSongItem));

  dom.artistsSection.hidden = artists.length === 0;
  dom.albumsSection.hidden = albums.length === 0;
  dom.songsSection.hidden = songs.length === 0;
  dom.artistsCount.textContent = plural(artists.length, "artista", "artistas");
  dom.albumsCount.textContent = plural(albums.length, "álbum", "álbuns");
  dom.songsCount.textContent = plural(songs.length, "música", "músicas");

  const total = artists.length + albums.length + songs.length;
  dom.title.textContent = `Resultados para “${query}”`;
  dom.total.textContent = plural(total, "resultado", "resultados");
  dom.results.hidden = total === 0;
  dom.empty.hidden = total !== 0;
  dom.hero.hidden = true;

  if (total === 0) {
    dom.empty.querySelector("h2").textContent = "Nenhum resultado encontrado";
    dom.empty.querySelector("p").textContent = `Não encontramos músicas, artistas ou álbuns iniciados por “${query}”.`;
    setStatus("");
  } else {
    setStatus(`Busca concluída. ${plural(total, "resultado encontrado", "resultados encontrados")}.`);
  }
}

async function runSearch(forcedQuery) {
  const query = String(forcedQuery ?? dom.input.value).trim();
  dom.input.value = query;
  dom.clear.hidden = query.length === 0;

  if (!query) {
    resetSearch();
    dom.input.focus();
    return;
  }

  const currentSequence = ++searchSequence;
  showLoading(query);

  const encoded = encodeURIComponent(query);
  const requests = await Promise.allSettled([
    apiGet(`/music/singer/${encoded}`),
    apiGet(`/music/album/${encoded}`),
    apiGet(`/music/name/${encoded}`)
  ]);

  if (currentSequence !== searchSequence) return;

  const successful = requests.filter((request) => request.status === "fulfilled");
  if (successful.length === 0) {
    dom.results.hidden = true;
    dom.empty.hidden = false;
    dom.empty.querySelector("h2").textContent = "Não foi possível acessar a API";
    dom.empty.querySelector("p").textContent = "Confirme que o backend está executando na porta 5000 e tente novamente.";
    setStatus("Falha ao conectar ao backend em http://localhost:5000.", "error");
    setApiStatus(false, "Backend desconectado");
    return;
  }

  const [artistsResult, albumsResult, songsResult] = requests;
  const artists = artistsResult.status === "fulfilled" ? artistsResult.value : [];
  const albums = albumsResult.status === "fulfilled" ? albumsResult.value : [];
  const songs = songsResult.status === "fulfilled" ? songsResult.value : [];

  setApiStatus(true, "Backend conectado");
  renderResults(query, artists, albums, songs);
}

function resetSearch() {
  searchSequence += 1;
  dom.input.value = "";
  dom.clear.hidden = true;
  dom.hero.hidden = false;
  dom.results.hidden = true;
  dom.empty.hidden = false;
  dom.empty.querySelector("h2").textContent = "Comece sua descoberta";
  dom.empty.querySelector("p").textContent = "Digite algo na barra de pesquisa para encontrar artistas, álbuns e músicas.";
  setStatus("");
}

async function checkApi() {
  try {
    const response = await fetch(`${API_PREFIX}/music/name/__rubra_health_check__`, {
      headers: { Accept: "application/json" },
      signal: AbortSignal.timeout(3500)
    });
    if (!response.ok) throw new Error("API indisponível");
    setApiStatus(true, "Backend conectado");
  } catch {
    setApiStatus(false, "Inicie a porta 5000");
  }
}

dom.form.addEventListener("submit", (event) => { event.preventDefault(); runSearch(); });
dom.input.addEventListener("input", () => { dom.clear.hidden = dom.input.value.length === 0; });
dom.clear.addEventListener("click", resetSearch);
document.querySelectorAll("[data-focus-search]").forEach((button) => {
  button.addEventListener("click", () => { dom.input.focus(); dom.input.scrollIntoView({ behavior: "smooth", block: "center" }); });
});

checkApi();
