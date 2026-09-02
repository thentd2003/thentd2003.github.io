/*
  Shared behavior for both pages (index.html = profile, projects.html = projects).
  Reads window.SITE_DATA from content.js. Edit content.js for copy — not this file.
  This file checks which elements exist on the current page and only renders those,
  so it's safe to include on both pages unchanged.
*/
(function(){
  var d = window.SITE_DATA;
  if(!d){
    document.body.innerHTML = '<p style="padding:40px;font-family:monospace;color:#eae6d9;">content.js failed to load — make sure it sits in the same folder as this file.</p>';
    return;
  }

  function el(html){
    var t = document.createElement('template');
    t.innerHTML = html.trim();
    return t.content.firstElementChild;
  }
  function isPlaceholder(text){
    return /^\s*\[.*\]\s*$/.test(String(text));
  }

  // ---------- Nav mark + resume link (both pages) ----------
  document.title = d.name + ' — Hardware Engineer';
  var navMark = document.getElementById('navMark');
  if(navMark){ navMark.innerHTML = d.name.split(' ').join('<span>_</span>').toUpperCase(); }

  var resumeLinkEl = document.getElementById('resumeLink');
  if(resumeLinkEl){ resumeLinkEl.setAttribute('href', d.resumeLink); }

  // ---------- Hero (profile page only) ----------
  if(document.getElementById('heroHeadline')){
    document.getElementById('heroHeadline').innerHTML =
      d.name + ' builds <em>' + d.headlineHighlight + '</em> underneath hardware.';
    document.getElementById('heroSub').textContent = d.tagline;

    var metaWrap = document.getElementById('heroMeta');
    d.meta.forEach(function(m){
      metaWrap.appendChild(el('<span class="chip">' + m + '</span>'));
    });
  }

  // ---------- Project card builder (shared) ----------
  function cardParts(p){
    var mediaHTML = p.image
      ? '<div class="project-media"><img src="' + p.image + '" alt="' + (p.imageAlt || '') + '"></div>'
      : '<div class="project-media project-media--empty"><span>' + (p.mediaLabel || '+ Add photo') + '</span></div>';
    var tagsHTML = p.tags.map(function(t){ return '<span class="tag">' + t + '</span>'; }).join('');
    var detailsHTML = Object.keys(p.details).map(function(k){
      var text = p.details[k];
      var cls = isPlaceholder(text) ? ' class="placeholder"' : '';
      return '<div class="details__block"><b>' + k + '</b><p' + cls + '>' + text + '</p></div>';
    }).join('');
    var footHTML =
      '<div class="card-foot">' +
        '<button class="toggle-btn" aria-expanded="false" data-toggle><span class="plus">+</span><span class="label"> Details</span></button>' +
        '<div class="link-row"><a href="' + (p.link || '#') + '" target="_blank" rel="noopener">' + (p.linkLabel || 'Link') + ' \u2197</a></div>' +
      '</div>';
    return { mediaHTML: mediaHTML, tagsHTML: tagsHTML, detailsHTML: detailsHTML, footHTML: footHTML };
  }

  // ---------- Project grids (projects page only) ----------
  var gridPower = document.getElementById('projectsGridPower');
  var gridEmbedded = document.getElementById('projectsGridEmbedded');
  if(gridPower || gridEmbedded){
    d.projects.forEach(function(p){
      var c = cardParts(p);
      var card = el(
        '<article class="project-card reveal">' +
          '<div class="project-card__top">' +
            '<span class="designator">' + p.designator + '</span>' +
            '<span class="status-pill ' + (p.statusClass || '') + '">' + p.status + '</span>' +
          '</div>' +
          c.mediaHTML +
          '<h3>' + p.title + '</h3>' +
          '<p class="blurb">' + p.blurb + '</p>' +
          '<div class="tag-row">' + c.tagsHTML + '</div>' +
          c.footHTML +
          '<div class="details"><div class="details__inner">' + c.detailsHTML + '</div></div>' +
        '</article>'
      );
      (p.section === 'embedded' ? gridEmbedded : gridPower).appendChild(card);
    });
  }

  // ---------- About (profile page only) ----------
  if(document.getElementById('aboutParagraphs')){
    var aboutP = document.getElementById('aboutParagraphs');
    d.about.paragraphs.forEach(function(t){
      aboutP.appendChild(el('<p>' + t + '</p>'));
    });

    var aboutFacts = document.getElementById('aboutFacts');
    if(aboutFacts){
      d.about.facts.forEach(function(f){
        aboutFacts.appendChild(el('<div><span>' + f[0] + '</span><span>' + f[1] + '</span></div>'));
      });
    }
  }

  // ---------- Footer / contact (both pages, if present) ----------
  if(document.getElementById('footerLinks')){
    document.getElementById('footerLinks').innerHTML =
      '<a class="btn-primary" href="mailto:' + d.email + '">Email</a>' +
      '<a class="btn-ghost" href="' + d.github + '" target="_blank" rel="noopener">GitHub</a>' +
      '<a class="btn-ghost" href="' + d.linkedin + '" target="_blank" rel="noopener">LinkedIn</a>';
    document.getElementById('footerName').textContent = d.name + ' — Hardware Engineer';
  }

  // ---------- Resume preview modal (both pages, if resume button present) ----------
  if(resumeLinkEl){
    var resumeModal = el(
      '<div class="resume-modal-overlay" role="dialog" aria-modal="true" aria-label="Resume preview">' +
        '<div class="resume-modal">' +
          '<div class="resume-modal__head">' +
            '<div class="resume-modal__label"><span>' + d.resumeLink + '</span></div>' +
            '<div class="resume-modal__actions">' +
              '<a class="resume-modal__download" href="' + d.resumeLink + '" download>Download ↓</a>' +
              '<button class="resume-modal__close" type="button" aria-label="Close">×</button>' +
            '</div>' +
          '</div>' +
          '<div class="resume-modal__body"><iframe src="' + d.resumeLink + '" title="Resume preview"></iframe></div>' +
          '<div class="resume-modal__fallback">Preview not loading? <a href="' + d.resumeLink + '" download>Download the PDF</a> instead.</div>' +
        '</div>' +
      '</div>'
    );
    document.body.appendChild(resumeModal);

    function openResumeModal(){
      resumeModal.classList.add('is-open');
      document.body.style.overflow = 'hidden';
      resumeModal.querySelector('.resume-modal__close').focus();
    }
    function closeResumeModal(){
      resumeModal.classList.remove('is-open');
      document.body.style.overflow = '';
    }

    resumeLinkEl.addEventListener('click', function(e){
      // Let modified clicks (open in new tab, middle-click, etc.) behave normally
      if(e.metaKey || e.ctrlKey || e.shiftKey || e.button === 1) return;
      e.preventDefault();
      openResumeModal();
    });
    resumeModal.querySelector('.resume-modal__close').addEventListener('click', closeResumeModal);
    resumeModal.addEventListener('click', function(e){
      if(e.target === resumeModal) closeResumeModal();
    });
    document.addEventListener('keydown', function(e){
      if(e.key === 'Escape' && resumeModal.classList.contains('is-open')) closeResumeModal();
    });
  }

  // ---------- Accordion (delegated, projects page) ----------
  document.addEventListener('click', function(e){
    var btn = e.target.closest('[data-toggle]');
    if(!btn) return;
    var card = btn.closest('.project-card');
    var details = card.querySelector('.details');
    var open = btn.getAttribute('aria-expanded') === 'true';
    btn.setAttribute('aria-expanded', String(!open));
    details.classList.toggle('is-open', !open);
    btn.querySelector('.label').textContent = open ? ' Details' : ' Close';
  });

  // ---------- Scroll reveal (both pages) ----------
  var revealEls = document.querySelectorAll('.reveal');
  if('IntersectionObserver' in window){
    var io = new IntersectionObserver(function(entries){
      entries.forEach(function(entry){
        if(entry.isIntersecting){
          entry.target.classList.add('is-visible');
          io.unobserve(entry.target);
        }
      });
    }, { threshold: 0.12 });
    revealEls.forEach(function(el){ io.observe(el); });
  } else {
    revealEls.forEach(function(el){ el.classList.add('is-visible'); });
  }
})();
